// Copyright (c) 2019-21 Branch Metrics, Inc.

#include "RequestManager.h"

#include <Poco/Net/PrivateKeyPassphraseHandler.h>
#include <Poco/Net/SSLManager.h>
#include <Poco/TaskNotification.h>
#include <cassert>

#include "BranchIO/Util/IClientSession.h"
#include "BranchIO/AdvertiserInfo.h"
#include "BranchIO/IPackagingInfo.h"
#include "BranchIO/Util/Log.h"
#include "BranchIO/Util/Storage.h"

using namespace Poco;
using Poco::Net::Context;

namespace BranchIO {

RequestManager::RequestManager(IPackagingInfo& packagingInfo, IClientSession *clientSession) :
    _thread("BranchIO:RequestManager"),
    _defaultCallback(nullptr),
    _packagingInfo(&packagingInfo),
    _clientSession(clientSession),
    _shuttingDown(false),
    _currentRequest(nullptr) {
    // https://pocoproject.org/docs/Poco.Net.Context.html
    /*
     * From <Poco/Net/Context.h>:
     * 	Context(Usage usage,
		const std::string& certificateNameOrPath, 
		VerificationMode verMode = VERIFY_RELAXED,
		int options = OPT_DEFAULTS,
		const std::string& certificateStoreName = CERT_STORE_MY);
			/// Creates a Context.
			/// 
			///   * usage specifies whether the context is used by a client or server,
			///     as well as which protocol to use.
			///   * certificateNameOrPath specifies either the subject name of the certificate to use,
			///     or the path of a PKCS #12 file containing the certificate and corresponding private key.
			///     If a subject name is specified, the certificate must be located in the certificate 
			///     store specified by certificateStoreName. If a path is given, the OPT_LOAD_CERT_FROM_FILE
			///     option must be set.
			///   * verificationMode specifies whether and how peer certificates are validated.
			///   * options is a combination of Option flags.
			///   * certificateStoreName specifies the name of the Windows certificate store
			///     to use for loading the certificate. Predefined constants
			///     CERT_STORE_MY, CERT_STORE_ROOT, etc. can be used.
			///
			/// Note: you can use OpenSSL to convert a certificate and private key in PEM format
			/// into PKCS #12 format required to import into the Context:
			///
			///     openssl pkcs12 -export -inkey cert.key -in cert.crt -out cert.pfx 

     */
    /*
     * Note that the following call will always generate an exception warning message in Visual Studio like:
     * Exception thrown at 0x00007FF87DB8D759 in TestBed-Local.exe: Microsoft C++ exception: Poco::Net::NoCertificateException at memory location 0x000000FD251FC1F0.
     * This is deliberately thrown by Poco internally when initializing a secure socket:
     * https://github.com/pocoproject/poco/blob/poco-1.10.1/NetSSL_Win/src/SecureSocketImpl.cpp#L692.
     * This is not a fatal error. API clients do not supply a cert for authentication.
     */
    Poco::Net::SSLManager::instance().initializeClient(0, 0, new Context(Context::TLS_CLIENT_USE, "" /* no client cert required */));
}

RequestManager::~RequestManager() {
    // Make sure the thread is terminated before we exit.
    stop();
    waitTillFinished();
}

RequestManager&
RequestManager::setDefaultCallback(IRequestCallback* callback) {
    Mutex::ScopedLock _l(_mutex);
    _defaultCallback = callback;
    return *this;
}

IRequestCallback*
RequestManager::getDefaultCallback() const {
    Mutex::ScopedLock _l(_mutex);
    return _defaultCallback;
}

RequestManager& RequestManager::enqueue(
    const BaseEvent& event,
    IRequestCallback* callback,
    bool urgent) {
    // Make a copy of the Request on the heap. This will throw if both
    // callback and getDefaultCallback() are NULL.
    RequestTask* task = new RequestTask(*this, event, callback ? callback : getDefaultCallback());
    TaskNotification* notification = new TaskNotification(task);
    Notification::Ptr notificationPtr = Notification::Ptr(notification);

    if (urgent) {
        _queue.enqueueUrgentNotification(notificationPtr);
    } else {
        _queue.enqueueNotification(notificationPtr);
    }

    return *this;
}

void RequestManager::start() {
    // execute run() on this thread
    _thread.start(*this);
}

void RequestManager::stop() {
    if (getCurrentRequest()) getCurrentRequest()->cancel();
    if (getClientSession()) getClientSession()->stop();

    if (!_thread.isRunning()) return;

    // Stop the background thread
    {
        Poco::Mutex::ScopedLock _l(_mutex);
        _shuttingDown = true;
    }

    _queue.wakeUpAll();
}

void
RequestManager::waitTillFinished() {
    _thread.join();
}

bool RequestManager::isShuttingDown() const {
    Mutex::ScopedLock _l(_mutex);
    return _shuttingDown;
}

void RequestManager::run() {
    try {
        while (!isShuttingDown()) {
            Notification::Ptr notificationPtr = _queue.waitDequeueNotification();
            // wakeUpAll() from stop() will return us from here with or without
            // a notification.
            if (isShuttingDown()) {
                break;  // from while; fall through past exception handlers
            }

            // We have an indefinite wait, so the only way we can get here is
            // if we have a notification.
            Notification* notification = notificationPtr.get();
            assert(notification);

            // We get away here with static_cast knowing everything is a
            // TaskNotification. Otherwise, we'll need a dynamic_cast.
            TaskNotification* taskNotification = static_cast<TaskNotification*>(notification);
            Task* task = taskNotification->task();
            RequestTask* requestTask = static_cast<RequestTask*>(task);

            setCurrentRequest(&requestTask->getRequest());
            task->runTask();
            setCurrentRequest(nullptr);
        }
    }
    catch (Poco::Exception& e) {
        // Poco exceptions
        BRANCH_LOG_E("Exception in RequestManager thread [" << e.what() << "]: " << e.message());
    }
    catch (std::exception& e) {
        // Other STL exceptions
        BRANCH_LOG_E("Exception in RequestManager thread: " << e.what());
    }
    catch (...) {
        // Anything else
        BRANCH_LOG_E("Unexpected exception in RequestManager thread.");
    }

    BRANCH_LOG_D("Terminating RequestManager thread.");
}

RequestManager::RequestTask::RequestTask(
    RequestManager& manager,
    const BaseEvent& event,
    IRequestCallback* callback) :
        Poco::Task("request"),
        _manager(manager),
        _event(event),
        _callback(callback) {
    if (!_callback) throw Poco::InvalidArgumentException("callback cannot be NULL.");
}

void
RequestManager::RequestTask::runTask() {
    JSONObject payload;
    _event.package(_manager.getPackagingInfo(), payload);

    if (_manager.getPackagingInfo().getAdvertiserInfo().isTrackingDisabled()) {
        payload.set(Defines::JSONKEY_TRACKING_DISABLED, true);
        // remove all identifiable fields
        // Based on https://github.com/BranchMetrics/ios-branch-deep-linking-attribution/blob/master/Branch-SDK/BNCServerInterface.m around line 400.
        payload.remove(Defines::JSONKEY_APP_DEVELOPER_IDENTITY);   // developer_identity
        payload.remove(Defines::JSONKEY_APP_IDENTITY);             // identity
        payload.remove(Defines::JSONKEY_DEVICE_LOCAL_IP_ADDRESS);  // local_ip
        payload.remove(Defines::JSONKEY_DEVICE_MAC_ADDRESS);       // mac_address
        payload.remove(Defines::JSONKEY_SESSION_FINGERPRINT);      // device_fingerprint_id
        payload.remove(Defines::JSONKEY_SESSION_IDENTITY);         // identity_id
        payload.remove("advertising_ids");
    }

    // Send request synchronously
    // _clientSession may be passed in for testing. If not, we
    // create/reuse a real one here
    JSONObject result;
    if (_manager.getClientSession()) {
        result = _request.send(_event.getAPIEndpoint(), payload, *_callback, _manager.getClientSession());
    } else {
        try {
            APIClientSession clientSession(BRANCH_IO_URL_BASE);
            _manager.setClientSession(&clientSession);
            result = _request.send(_event.getAPIEndpoint(), payload, *_callback, &clientSession);
            _manager.setClientSession(nullptr);
        }
        catch (Poco::Exception& e) {
            BRANCH_LOG_E("Connection failed. " << e.what() << ": " << e.message());
        }
    }

    _event.handleResult(result);
}

}  // namespace BranchIO
