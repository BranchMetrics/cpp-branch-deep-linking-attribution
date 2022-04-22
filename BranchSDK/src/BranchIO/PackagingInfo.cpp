// Copyright (c) 2019-21 Branch Metrics, Inc.

#include "BranchIO/PackagingInfo.h"

using namespace std;

namespace BranchIO {

std::string
PackagingInfo::getBranchKey() const {
    scoped_lock _l(_mutex);
    return _branchKey;
}

PackagingInfo&
PackagingInfo::setBranchKey(const std::string& branchKey) {
    scoped_lock _l(_mutex);
    _branchKey = branchKey;
    return *this;
}

SessionInfo&
PackagingInfo::getSessionInfo() {
    scoped_lock _l(_mutex);
    return _sessionInfo;
}

const SessionInfo&
PackagingInfo::getSessionInfo() const {
    scoped_lock _l(_mutex);
    return _sessionInfo;
}

PackagingInfo&
PackagingInfo::setSessionInfo(const SessionInfo& sessionInfo) {
    scoped_lock _l(_mutex);
    _sessionInfo = sessionInfo;
    return *this;
}

const AppInfo&
PackagingInfo::getAppInfo() const {
    scoped_lock _l(_mutex);
    return _appInfo;
}

AppInfo&
PackagingInfo::getAppInfo() {
    scoped_lock _l(_mutex);
    return _appInfo;
}

PackagingInfo&
PackagingInfo::setAppInfo(const AppInfo& appInfo) {
    scoped_lock _l(_mutex);
    _appInfo = appInfo;
    return *this;
}

const DeviceInfo&
PackagingInfo::getDeviceInfo() const {
    scoped_lock _l(_mutex);
    return _deviceInfo;
}

PackagingInfo&
PackagingInfo::setDeviceInfo(const DeviceInfo& deviceInfo) {
    scoped_lock _l(_mutex);
    _deviceInfo = deviceInfo;
    return *this;
}

AdvertiserInfo&
PackagingInfo::getAdvertiserInfo() {
    scoped_lock _l(_mutex);
    return _advertiserInfo;
}

PackagingInfo&
PackagingInfo::setAdvertiserInfo(const AdvertiserInfo& advertiserInfo) {
    scoped_lock _l(_mutex);
    _advertiserInfo = advertiserInfo;
    return *this;
}

void PackagingInfo::setRequestMetaData(JSONObject requestMetaData){
    scoped_lock _l(_mutex);
    _requestMetaData = requestMetaData;
}

JSONObject& PackagingInfo::getRequestMetaData()
{
    scoped_lock _l(_mutex);
    return _requestMetaData;
}

}  // namespace BranchIO
