// Copyright (c) 2019-21 Branch Metrics, Inc.

#include "RequestManager.h"

#include <cassert>

#include "BranchIO/Util/IClientSession.h"
#include "BranchIO/AdvertiserInfo.h"
#include "BranchIO/IPackagingInfo.h"
#include "BranchIO/Util/Log.h"
#include "BranchIO/Util/Storage.h"


namespace BranchIO {

RequestManager::RequestManager(IPackagingInfo& packagingInfo, IClientSession *clientSession) :
    _defaultCallback(nullptr),
    _packagingInfo(&packagingInfo),
    _clientSession(clientSession),
    _shuttingDown(false),
    _currentRequest(nullptr) {
}

RequestManager::~RequestManager() {
    // Make sure the thread is terminated before we exit.
    stop();
    waitTillFinished();
}

RequestManager&
RequestManager::setDefaultCallback(IRequestCallback* callback) {
    std::scoped_lock _l(_mutex);
    _defaultCallback = callback;
    return *this;
}

IRequestCallback*
RequestManager::getDefaultCallback() const {
    std::scoped_lock _l(_mutex);
    return _defaultCallback;
}

RequestManager& RequestManager::enqueue(
    const BaseEvent& event,
    IRequestCallback* callback,
    bool urgent) {
    // Make a copy of the Request on the heap. This will throw if both
    // callback and getDefaultCallback() are NULL.
    RequestTask* task = new RequestTask(*this, event, callback ? callback : getDefaultCallback());

    if (urgent) {
        enqueueUrgentTask(task);
    } else {
        enqueueTask(task);
    }

    return *this;
}

void RequestManager::start() {
    // start background thread for sending events to server
    _thread = std::thread(&RequestManager::run, this);
}

void RequestManager::stop() {
    if (getCurrentRequest()) getCurrentRequest()->cancel();
    if (getClientSession()) getClientSession()->stop();

    if (!_thread.joinable()) return;

    // Stop the background thread
    {
        std::scoped_lock _l(_mutex);
        _shuttingDown = true;
    }

    wakeUpAll();
}

void
RequestManager::waitTillFinished() {
    _thread.join();
}

bool RequestManager::isShuttingDown() const {
    std::scoped_lock _l(_mutex);
    return _shuttingDown;
}

void RequestManager::run() {
    try {
        while (!isShuttingDown()) {
            RequestTask* requestTask = waitDequeueNotification();
            // wakeUpAll() from stop() will return us from here with or without
            // a notification.
            if (isShuttingDown()) {
                break;  // from while; fall through past exception handlers
            }

            // We have an indefinite wait, so the only way we can get here is
            // if we have a notification.
            setCurrentRequest(&requestTask->getRequest());
            requestTask->runTask();
            setCurrentRequest(nullptr);
        }
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
        _manager(manager),
        _event(event),
        _callback(callback) {
    if (!_callback) throw std::exception("InvalidArgumentException - callback cannot be NULL.");
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
        catch (winrt::hresult_error const& e) {
            BRANCH_LOG_E("Connection failed. " << e.code() << ": " << e.message().c_str());
        }
    }

    _event.handleResult(result);
}

void RequestManager::enqueueTask(RequestTask* task)
{
    std::scoped_lock  lock(_mutex);
    _queue.push_back(task);
    _available.notify_one();
}

void RequestManager::enqueueUrgentTask(RequestTask* task)
{
    std::scoped_lock  lock(_mutex);
    _queue.push_front(task);
    _available.notify_one();
}


RequestManager::RequestTask* RequestManager::waitDequeueNotification()
{
    std::unique_lock<std::mutex>  lock(_mutex);
    _available.wait(lock, [=] { return !_queue.empty(); });
   
    RequestManager::RequestTask* task =  _queue.front();
    _queue.pop_front();

    return task;
}

void RequestManager::wakeUpAll()
{
    std::scoped_lock lock(_mutex);
    _available.notify_all();
}


}  // namespace BranchIO
