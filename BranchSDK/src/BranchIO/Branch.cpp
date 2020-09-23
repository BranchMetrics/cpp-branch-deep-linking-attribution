// Copyright (c) 2019 Branch Metrics, Inc.

#include "BranchIO/Branch.h"

#include <string>

#include "BranchIO/DeviceInfo.h"
#include "BranchIO/Event/Event.h"
#include "BranchIO/Event/IdentityEvent.h"
#include "BranchIO/Event/SessionEvent.h"
#include "BranchIO/IRequestCallback.h"
#include "BranchIO/Util/Log.h"
#include "BranchIO/SessionInfo.h"
#include "BranchIO/Util/Storage.h"
#include "BranchIO/Version.h"
#include "BranchIO/Util/RequestManager.h"

// Create a string that looks like this:  "Branch Windows SDK v1.2.3"
#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)
#define VER_FILE_VERSION_STR        STRINGIZE(BRANCHIO_VERSION_MAJOR)        \
                                    "." STRINGIZE(BRANCHIO_VERSION_MINOR)    \
                                    "." STRINGIZE(BRANCHIO_VERSION_REVISION)

#define VER_FILE_VERSION_DISPLAY    "Branch "                       \
                                    VERSION_PLATFORM  " SDK v"      \
                                    VER_FILE_VERSION_STR

namespace BranchIO {

#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
#define VERSION_PLATFORM  "Unix"
class BranchUnix : public Branch {
 public:
    BranchUnix() : Branch() {}
    virtual ~BranchUnix() {}
};

#elif defined(_WIN64) || defined(_WIN32)
#define VERSION_PLATFORM  "Windows"
class BranchWindows : public Branch {
 public:
    BranchWindows() : Branch() {}
    virtual ~BranchWindows() {}
};

#endif

using namespace std;

/**
 * (Internal) Session Callback.
 * This class maintains the state of the IPackagingInfo Session
 */
class SessionCallback : public IRequestCallback {
 public:
    /**
     * Constructor.
     * @param context IPackagingInfo context
     * @param parent Parent callback to be called after this has processed the response.
     */
    SessionCallback(IPackagingInfo *context, IRequestCallback *parent) :
        _context(context), _parentCallback(parent) {}

    virtual void onSuccess(int id, JSONObject jsonResponse) {
        // @todo(andyp): Update Branch State
        if (_context) {
            // If keys exist, we set them on the Session Context.
            // If keys don't exist -- that effectively wipes out the state (on purpose).
            if (jsonResponse.has(Defines::JSONKEY_SESSION_ID)) {
                if (!_context->getAdvertiserInfo().isTrackingDisabled()) {
                    _context->getSessionInfo().setFingerprintId(jsonResponse.get(Defines::JSONKEY_SESSION_FINGERPRINT));
                    _context->getSessionInfo().setIdentityId(jsonResponse.get(Defines::JSONKEY_SESSION_IDENTITY));
                }
                _context->getSessionInfo().setSessionId(jsonResponse.get(Defines::JSONKEY_SESSION_ID));
            }

            // Data comes back as String-encoded JSON...  let's fix that up
            if (jsonResponse.has("data")) {
                try {
                    std::string stringData = jsonResponse.get("data");
                    JSONObject jsonData = JSONObject::parse(stringData);
                    jsonResponse.set("data", jsonData);
                }
                catch(Poco::Exception &) {
                    // Error has already been logged.
                }
            }

            BRANCH_LOG_D("Branch::SessionCallback() Success\tJSON: " << jsonResponse.stringify());
        }

        // Call the original callback
        if (_parentCallback) {
            _parentCallback->onSuccess(id, jsonResponse);
        }
    }

    virtual void onError(int id, int error, std::string description) {
        if (_parentCallback) {
            _parentCallback->onError(id, error, description);
        }
    }

    virtual void onStatus(int id, int error, std::string description) {
        if (_parentCallback) {
            _parentCallback->onStatus(id, error, description);
        }
    }

 private:
    IPackagingInfo *_context;
    IRequestCallback *_parentCallback;
};

Branch *Branch::create(const String& branchKey, AppInfo* pInfo) {
    Branch *instance = nullptr;
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
    instance = new BranchUnix();
#elif defined(_WIN64) || defined(_WIN32)
    instance = new BranchWindows();
#endif

    /*
     * For now, we should use user-level storage (~/.branchio on Unix,
     * HKEY_CURRENT_USER on Windows) to avoid permission issues. Some
     * fields could conceivably be stored at the Host level. That can
     * be overridden on a field-by-field basis using the optional
     * scope field. This API can also evolve.
     *
     * Storage::instance().set(key, value, Storage::Host);
     */
    IStorage& storage(Storage::instance());
    storage.setDefaultScope(Storage::User);
    storage.setPrefix(branchKey);

    // operator new does not return NULL. It throws std::bad_alloc in case of
    // failure. no need to check this pointer.
    if (pInfo) {
        instance->_packagingInfo.getAppInfo().addProperties(pInfo->toJSON());
    }

    instance->_packagingInfo.setBranchKey(branchKey.str());

    instance->_requestManager = new RequestManager(instance->_packagingInfo);
    instance->_requestManager->start();

    return instance;
}

Branch::Branch() {}
Branch::~Branch() {
    delete _requestManager;
}

void
Branch::openSession(const String& linkUrl, IRequestCallback* callback) {
    SessionCallback *sessionCallback = new SessionCallback(&_packagingInfo, callback);
    SessionOpenEvent event;

    string sLinkUrl(linkUrl.str());
    if (sLinkUrl.length() > 0) {
        event.setLinkUrl(sLinkUrl);
    }

    sendEvent(event, sessionCallback);
}

void
Branch::closeSession(IRequestCallback *callback) {
    SessionCallback *sessionCallback = new SessionCallback(&_packagingInfo, callback);
    SessionCloseEvent event;
    sendEvent(event, sessionCallback);
}

void
Branch::sendEvent(const BaseEvent &event, IRequestCallback *callback) {
    // Only open events are enqueued with tracking disabled. All tracking info is stripped out
    // before transmission.
    if (getAdvertiserInfo().isTrackingDisabled() && event.getAPIEndpoint() != Defines::REGISTER_OPEN) {
        if (callback) {
            callback->onStatus(0, 0, "Requested operation cannot be completed since tracking is disabled");
            callback->onError(0, 0, "Tracking is disabled");
        }
        return;
    }

    getRequestManager()->enqueue(event, callback);
}

void
Branch::setIdentity(const String& userId, IRequestCallback* callback) {
    if (getSessionInfo().hasSessionId()) {
        IdentityLoginEvent event(userId.str());
        sendEvent(event, callback);
    } else {
        if (callback) {
            callback->onError(0, 0, "No Session has been started.");
        }
    }
}

void
Branch::logout(IRequestCallback *callback) {
    if (getSessionInfo().hasSessionId()) {
        IdentityLogoutEvent event;
        sendEvent(event, callback);
    } else {
        if (callback) {
            callback->onError(0, 0, "No Session has been started.");
        }
    }
}

void
Branch::stop() {
    getRequestManager()->stop();
}

void
Branch::waitTillFinished() {
    getRequestManager()->waitTillFinished();
}

const AppInfo &
Branch::getAppInfo() const {
    Poco::Mutex::ScopedLock _l(_mutex);
    return _packagingInfo.getAppInfo();
}

const DeviceInfo &
Branch::getDeviceInfo() const {
    Poco::Mutex::ScopedLock _l(_mutex);
    return _packagingInfo.getDeviceInfo();
}

SessionInfo &
Branch::getSessionInfo() {
    Poco::Mutex::ScopedLock _l(_mutex);
    return _packagingInfo.getSessionInfo();
}

SessionInfo const &
Branch::getSessionInfo() const {
    Poco::Mutex::ScopedLock _l(_mutex);
    return _packagingInfo.getSessionInfo();
}

AdvertiserInfo &
Branch::getAdvertiserInfo() {
    Poco::Mutex::ScopedLock _l(_mutex);
    return _packagingInfo.getAdvertiserInfo();
}

RequestManager *
Branch::getRequestManager() const {
    Poco::Mutex::ScopedLock _l(_mutex);
    return _requestManager;
}

std::string
Branch::getBranchKey() const {
    Poco::Mutex::ScopedLock _l(_mutex);
    return _packagingInfo.getBranchKey();
}

std::string Branch::getVersion() {
    return VER_FILE_VERSION_STR;
}

#ifdef WIN32

std::wstring Branch::getVersionW() {
    return String(getVersion()).wstr();
}

std::wstring Branch::getBranchKeyW() const {
    return String(getBranchKey()).wstr();
}

#endif  // WIN32

}  // namespace BranchIO
