// Copyright (c) 2019 Branch Metrics, Inc.

#include "BranchIO/Branch.h"

#include <string>

#include "BranchIO/DeviceInfo.h"
#include "BranchIO/Event/Event.h"
#include "BranchIO/Event/SessionEvent.h"
#include "BranchIO/IRequestCallback.h"
#include "BranchIO/Util/Log.h"
#include "BranchIO/SessionInfo.h"
#include "BranchIO/Storage.h"
#include "BranchIO/Version.h"
#include "BranchIO/RequestManager.h"

// Create a string that looks like this:  "Branch Windows SDK v1.2.3"
#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)
#define VER_FILE_VERSION_STR        STRINGIZE(VERSION_MAJOR)        \
                                    "." STRINGIZE(VERSION_MINOR)    \
                                    "." STRINGIZE(VERSION_REVISION)

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
        if (_context != NULL) {
            // If keys exist, we set them on the Session Context.
            // If keys don't exist -- that effectively wipes out the state (on purpose).
            if (jsonResponse.has(Defines::JSONKEY_SESSION_ID)) {
                _context->getSessionInfo().setFingerprintId(jsonResponse.get(Defines::JSONKEY_SESSION_FINGERPRINT));
                _context->getSessionInfo().setSessionId(jsonResponse.get(Defines::JSONKEY_SESSION_ID));
                _context->getSessionInfo().setIdentityId(jsonResponse.get(Defines::JSONKEY_SESSION_IDENTITY));
            }

            // Data comes back as String-encoded JSON...  let's fix that up
            if (jsonResponse.has("data")) {
                try {
                    std::string stringData = jsonResponse.get("data");
                    JSONObject::Ptr jsonData = JSONObject::parse(stringData);
                    jsonResponse.set("data", jsonData);
                }
                catch(Poco::Exception &e) {
                    // Error has already been logged.
                }
            }

            BRANCH_LOG_D("Branch::SessionCallback() Success\tJSON: " << jsonResponse.stringify());
        }

        // Call the original callback
        if (_parentCallback != NULL) {
            _parentCallback->onSuccess(id, jsonResponse);
        }
    }

    virtual void onError(int id, int error, std::string description) {
        if (_parentCallback != NULL) {
            _parentCallback->onError(id, error, description);
        }
    }

    virtual void onStatus(int id, int error, std::string description) {
        if (_parentCallback != NULL) {
            _parentCallback->onStatus(id, error, description);
        }
    }

 private:
    IPackagingInfo *_context;
    IRequestCallback *_parentCallback;
};

Branch *Branch::create(const std::string &branchKey, AppInfo *pInfo) {
    Branch *instance = NULL;
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
    Storage& storage(Storage::instance());
    storage.setDefaultScope(Storage::User);

    // operator new does not return NULL. It throws std::bad_alloc in case of
    // faliure. no need to check this pointer.
    if (pInfo != NULL) {
        instance->_packagingInfo.getAppInfo().addProperties(pInfo->toJSON());
    }

    instance->_packagingInfo.setBranchKey(branchKey);

    instance->_requestManager = new RequestManager(instance->_packagingInfo);
    instance->_requestManager->start();

    return instance;
}

Branch::Branch() {}
Branch::~Branch() {
    delete _requestManager;
}

void
Branch::openSession(const std::string &linkUrl, IRequestCallback *callback) {
    SessionCallback *sessionCallback = new SessionCallback(&_packagingInfo, callback);
    SessionOpenEvent event;

    if (linkUrl.length() > 0) {
        event.setLinkUrl(linkUrl);
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
Branch::sendEvent(const Event &event, IRequestCallback *callback) {
    if (getAdvertiserInfo().isTrackingDisabled()) {
        if (callback != NULL) {
            callback->onStatus(0, 0, "Requested operation cannot be completed since tracking is disabled");
            callback->onError(0, 0, "Tracking is disabled");
        }
        return;
    }

    getRequestManager()->enqueue(event, callback);
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

const std::string &Branch::getVersion() {
    static std::string sVersion = VER_FILE_VERSION_STR;
    return sVersion;
}

}  // namespace BranchIO
