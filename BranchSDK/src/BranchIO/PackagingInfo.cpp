// Copyright (c) 2019 Branch Metrics, Inc.

#include "BranchIO/PackagingInfo.h"

using Poco::Mutex;

namespace BranchIO {

std::string
PackagingInfo::getBranchKey() const {
    Mutex::ScopedLock _l(_mutex);
    return _branchKey;
}

PackagingInfo&
PackagingInfo::setBranchKey(const std::string& branchKey) {
    Mutex::ScopedLock _l(_mutex);
    _branchKey = branchKey;
    return *this;
}

SessionInfo&
PackagingInfo::getSessionInfo() {
    Mutex::ScopedLock _l(_mutex);
    return _sessionInfo;
}

const SessionInfo&
PackagingInfo::getSessionInfo() const {
    Mutex::ScopedLock _l(_mutex);
    return _sessionInfo;
}

PackagingInfo&
PackagingInfo::setSessionInfo(const SessionInfo& sessionInfo) {
    Mutex::ScopedLock _l(_mutex);
    _sessionInfo = sessionInfo;
    return *this;
}

const AppInfo&
PackagingInfo::getAppInfo() const {
    Mutex::ScopedLock _l(_mutex);
    return _appInfo;
}

AppInfo&
PackagingInfo::getAppInfo() {
    Mutex::ScopedLock _l(_mutex);
    return _appInfo;
}

PackagingInfo&
PackagingInfo::setAppInfo(const AppInfo& appInfo) {
    Mutex::ScopedLock _l(_mutex);
    _appInfo = appInfo;
    return *this;
}

const DeviceInfo&
PackagingInfo::getDeviceInfo() const {
    Mutex::ScopedLock _l(_mutex);
    return _deviceInfo;
}

PackagingInfo&
PackagingInfo::setDeviceInfo(const DeviceInfo& deviceInfo) {
    Mutex::ScopedLock _l(_mutex);
    _deviceInfo = deviceInfo;
    return *this;
}

AdvertiserInfo&
PackagingInfo::getAdvertiserInfo() {
    Mutex::ScopedLock _l(_mutex);
    return _advertiserInfo;
}

PackagingInfo&
PackagingInfo::setAdvertiserInfo(const AdvertiserInfo& advertiserInfo) {
    Mutex::ScopedLock _l(_mutex);
    _advertiserInfo = advertiserInfo;
    return *this;
}

}  // namespace BranchIO
