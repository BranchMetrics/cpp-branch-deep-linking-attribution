// Copyright (c) 2019 Branch Metrics, Inc.

#include "BranchIO/SessionInfo.h"
#include "BranchIO/Defines.h"
#include "BranchIO/Util/Log.h"
#include "BranchIO/Util/Storage.h"

using std::string;

namespace BranchIO {

SessionInfo::SessionInfo() {
    // Load these fields from storage if present.
    std::string deviceFingerprint =
            Storage::instance().getString(getPath(SESSIONSTORAGE, Defines::JSONKEY_SESSION_FINGERPRINT));
    std::string identityId =
            Storage::instance().getString(getPath(SESSIONSTORAGE, Defines::JSONKEY_SESSION_IDENTITY));

    if (!deviceFingerprint.empty()) {
        doAddProperty(Defines::JSONKEY_SESSION_FINGERPRINT, deviceFingerprint);
    }

    if (!identityId.empty()) {
        doAddProperty(Defines::JSONKEY_SESSION_IDENTITY, identityId);
    }
}

SessionInfo::~SessionInfo() = default;

SessionInfo&
SessionInfo::setFingerprintId(const std::string &deviceFingerprint) {
    Storage::instance().setString(getPath(SESSIONSTORAGE, Defines::JSONKEY_SESSION_FINGERPRINT), deviceFingerprint);
    return doAddProperty(Defines::JSONKEY_SESSION_FINGERPRINT, deviceFingerprint);
}

SessionInfo&
SessionInfo::setIdentityId(const std::string &identityId) {
    return doAddProperty(Defines::JSONKEY_SESSION_IDENTITY, identityId);
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
