// Copyright (c) 2019-21 Branch Metrics, Inc.

#include "BranchIO/Branch.h"

#include <string>

using namespace std;

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

class BranchWindows : public Branch {
 public:
    BranchWindows() : Branch() {}
    virtual ~BranchWindows() {}
};

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
                    if (jsonResponse.has(Defines::JSONKEY_SESSION_FINGERPRINT)) {
                        static const char* const key = Defines::JSONKEY_SESSION_FINGERPRINT;
                        string deviceFingerprintId(jsonResponse.get(key).toString());
                        _context->getSessionInfo().setFingerprintId(deviceFingerprintId);
                    }
                    if (jsonResponse.has(Defines::JSONKEY_SESSION_IDENTITY))
                        _context->getSessionInfo().setIdentityId(jsonResponse.get(Defines::JSONKEY_SESSION_IDENTITY));
                }
                _context->getSessionInfo().setSessionId(jsonResponse.get(Defines::JSONKEY_SESSION_ID));
            }

            // Data comes back as String-encoded JSON...  let's fix that up
            if (jsonResponse.has("data")) {
                try {
                    string stringData = jsonResponse.get("data");
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

        // Either onSuccess or onError is guaranteed to be called only once on request completion.
        done();
    }

    virtual void onError(int id, int error, string description) {
        if (_parentCallback) {
            _parentCallback->onError(id, error, description);
        }

        // Either onSuccess or onError is guaranteed to be called only once on request completion.
        done();
    }

    virtual void onStatus(int id, int error, string description) {
        if (_parentCallback) {
            _parentCallback->onStatus(id, error, description);
        }
    }

 private:
    void done() {
        delete this;
    }

    IPackagingInfo *_context;
    IRequestCallback *_parentCallback;
};

Branch *Branch::create(const String& branchKey, AppInfo* pInfo) {
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

    // Migrate global settings from old builds (before setting prefix). Assume these are from
    // a previous installation of the same app.
    storage.setPrefix("");  // in case Branch::create called more than once.
    bool hasGlobalTrackingDisabled = storage.has("advertiser.trackingDisabled");
    bool isGlobalTrackingDisabled = storage.getBoolean("advertiser.trackingDisabled");
    string globalDeviceFingerprintId = storage.getString("session.device_fingerprint_id");

    // Remove global settings
    /*
     * In case more than one app with an older version of this SDK disabled tracking in the past,
     * do not remove this old global setting for advertiser.trackingDisabled. All apps updating
     * to this version of the SDK will import the older global setting.
     */
    // storage.remove("advertiser");
    storage.remove("session");

    auto utf8key(branchKey.str());
    storage.setPrefix(utf8key);

    // Must initialize Branch object after prefix set.
    Branch* instance = nullptr;
    instance = new BranchWindows();

    // Set these on the current app
    if (hasGlobalTrackingDisabled && !storage.has("advertiser.trackingDisabled")) {
        storage.setBoolean("advertiser.trackingDisabled", isGlobalTrackingDisabled);
    }
    if (!globalDeviceFingerprintId.empty() && !storage.has("session.device_fingerprint_id")) {
        storage.setString("session.device_fingerprint_id", globalDeviceFingerprintId);
    }

    if (pInfo) {
        instance->_packagingInfo.getAppInfo().addProperties(pInfo->toJSON());
    }

    instance->_packagingInfo.setBranchKey(utf8key);

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
    BRANCH_LOG_D("Branch::closeSession is a no-op. It is not necessary to call this function.");

    if (!callback) return;

    callback->onStatus(0, 0, "Branch::closeSession is a no-op. It is not necessary to call this function.");
    callback->onSuccess(0, JSONObject());
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
        event.setResultHandler([this, userId](const JSONObject& result) {
            /*
             * Note that setIdentity just generates an error via sendEvent below
             * if tracking is disabled. This callback is only invoked on successful
             * completion of the request.
             */
            IStorage& storage(Storage::instance());
            storage.setString("session.identity", userId.str());
            if (result.has(Defines::JSONKEY_SESSION_ID)) {
                auto sessionId = result.get(Defines::JSONKEY_SESSION_ID).toString();
                getSessionInfo().setSessionId(sessionId);
            }
            if (result.has(Defines::JSONKEY_SESSION_IDENTITY)) {
                auto identityId = result.get(Defines::JSONKEY_SESSION_IDENTITY).toString();
                getSessionInfo().setIdentityId(identityId);
                storage.setString("session.identity_id", identityId);
            }
        });
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
        event.setResultHandler([this](const JSONObject& result) {
            IStorage& storage(Storage::instance());
            storage.remove("session.identity");
            if (result.has(Defines::JSONKEY_SESSION_ID)) {
                auto sessionId = result.get(Defines::JSONKEY_SESSION_ID).toString();
                getSessionInfo().setSessionId(sessionId);
            }
            if (result.has(Defines::JSONKEY_SESSION_IDENTITY)) {
                auto identityId = result.get(Defines::JSONKEY_SESSION_IDENTITY).toString();
                getSessionInfo().setIdentityId(identityId);
                storage.setString("session.identity_id", identityId);
            }
        });
        sendEvent(event, callback);
    } else {
        if (callback) {
            callback->onError(0, 0, "No Session has been started.");
        }
    }
}

string
Branch::getIdentity() {
    return Storage::instance().getString("session.identity");
}

wstring
Branch::getIdentityW() {
    return String(getIdentity()).wstr();
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

string
Branch::getBranchKey() const {
    Poco::Mutex::ScopedLock _l(_mutex);
    return _packagingInfo.getBranchKey();
}

string Branch::getVersion() {
    return VER_FILE_VERSION_STR;
}

wstring Branch::getVersionW() {
    return String(getVersion()).wstr();
}

wstring Branch::getBranchKeyW() const {
    return String(getBranchKey()).wstr();
}

}  // namespace BranchIO
