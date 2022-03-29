// Copyright (c) 2019-21 Branch Metrics, Inc.

#include "BranchIO/SessionInfo.h"
#include "BranchIO/Defines.h"
#include "BranchIO/Util/Log.h"
#include "BranchIO/Util/Storage.h"

using std::string;

namespace BranchIO {

SessionInfo::SessionInfo() {
    
    // Load these fields from storage if present.
    
    std::string deviceToken = Storage::instance().getString(getPath(SESSIONSTORAGE, Defines::JSONKEY_SESSION_RANDOMIZED_DEVICE_TOKEN));
    // device_fingerprint_id has been renamed to randomized_device_token.
    // So, for backward comatibility, check for stored device_fingerprint_id also if randomized_device_token not found.
    if (deviceToken.empty()) {
        deviceToken = Storage::instance().getString(getPath(SESSIONSTORAGE, Defines::JSONKEY_SESSION_FINGERPRINT));
    }
    
    std::string bundleToken = Storage::instance().getString(getPath(SESSIONSTORAGE, Defines::JSONKEY_SESSION_RANDOMIZED_BUNDLE_TOKEN));
    // identity_id has been renamed to randomized_bundle_token.
    // So, for backward comatibility, check for stored identity_id also if randomized_bundle_token not found.
    if (bundleToken.empty()) {
        bundleToken = Storage::instance().getString(getPath(SESSIONSTORAGE, Defines::JSONKEY_SESSION_IDENTITY));
    }

    if (!deviceToken.empty()) {
        doAddProperty(Defines::JSONKEY_SESSION_RANDOMIZED_DEVICE_TOKEN, deviceToken);
    }

    if (!bundleToken.empty()) {
        doAddProperty(Defines::JSONKEY_SESSION_RANDOMIZED_BUNDLE_TOKEN, bundleToken);
    }
}

SessionInfo::~SessionInfo() = default;

SessionInfo&
SessionInfo::setDeviceToken(const std::string & randomizedDeviceToken) {
    Storage::instance().setString(getPath(SESSIONSTORAGE, Defines::JSONKEY_SESSION_RANDOMIZED_DEVICE_TOKEN), randomizedDeviceToken);
    return doAddProperty(Defines::JSONKEY_SESSION_RANDOMIZED_DEVICE_TOKEN, randomizedDeviceToken);
}

SessionInfo&
SessionInfo::setBundleToken(const std::string &bundleToken) {
    return doAddProperty(Defines::JSONKEY_SESSION_RANDOMIZED_BUNDLE_TOKEN, bundleToken);
}

SessionInfo&
SessionInfo::setSessionId(const std::string &sessionId) {
    return doAddProperty(Defines::JSONKEY_SESSION_ID, sessionId);
}

bool
SessionInfo::hasSessionId() const {
    return has(Defines::JSONKEY_SESSION_ID);
}

SessionInfo&
SessionInfo::doAddProperty(const char *name, const std::string &value) {
    addProperty(name, value);
    return *this;
}

}  // namespace BranchIO
