// Copyright (c) 2019 Branch Metrics, Inc.

#include "BranchIO/SessionInfo.h"

#include "BranchIO/Defines.h"
#include "BranchIO/Util/Storage.h"

using std::string;

namespace BranchIO {

SessionInfo::SessionInfo() {
    /**
     * Load these fields from storage if present.
     */
    load(Defines::JSONKEY_SESSION_FINGERPRINT);
}

SessionInfo::~SessionInfo() {
}

SessionInfo&
SessionInfo::setFingerprintId(const std::string &deviceFingerprint) {
    save(Defines::JSONKEY_SESSION_FINGERPRINT, deviceFingerprint);
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

SessionInfo&
SessionInfo::doAddProperty(const char *name, const std::string &value) {
    addProperty(name, value);
    return *this;
}

std::string
SessionInfo::getStoragePath(const char* key) {
    string storagePath(StoragePrefix);
    storagePath += ".";
    storagePath += key;
    return storagePath;
}

void
SessionInfo::load(const char* key) {
    string value;

    string storagePath(getStoragePath(key));
    if (Storage::instance().get(storagePath, value)) {
        addProperty(key, value);
    }
}

void
SessionInfo::save(const char *key, const std::string &value) {
    Storage::instance().set(getStoragePath(key), value);
}

}  // namespace BranchIO
