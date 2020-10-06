// Copyright (c) 2019 Branch Metrics, Inc.

#include "BranchIO/AppInfo.h"

#include "BranchIO/Defines.h"
#include "BranchIO/JSONObject.h"
#include "BranchIO/Util/Log.h"

namespace BranchIO {

std::string getOsName();

AppInfo::AppInfo() : PropertyManager() {
}

AppInfo::AppInfo(const JSONObject &jsonObject)
    : PropertyManager(jsonObject) {
}

AppInfo::~AppInfo() {
}

AppInfo&
AppInfo::setAppVersion(const String& appVersion) {
    return doAddProperty(Defines::JSONKEY_APP_VERSION, appVersion.str());
}

#pragma warning(disable: 4995)
AppInfo&
AppInfo::setDeveloperIdentity(const String& identity) {
    BRANCH_LOG_W("AppInfo::setDeveloperIdentity is deprecated. Please use Branch::setIdentity instead.");
}
#pragma warning(default: 4995)

AppInfo&
AppInfo::setEnvironment(const String& environment) {
    return doAddProperty(Defines::JSONKEY_APP_ENVIRONMENT, environment.str());
}

AppInfo&
AppInfo::setCountryCode(const String& countryCode) {
    return doAddProperty(Defines::JSONKEY_DEVICE_COUNTRY, countryCode.str());
}

AppInfo&
AppInfo::setDisplayInfo(int dpi, int width, int height) {
    doAddProperty(Defines::JSONKEY_DEVICE_SCREEN_DPI, dpi);
    doAddProperty(Defines::JSONKEY_DEVICE_SCREEN_WIDTH, width);
    doAddProperty(Defines::JSONKEY_DEVICE_SCREEN_HEIGHT, height);

    return *this;
}

AppInfo&
AppInfo::setLanguage(const String& language) {
    return doAddProperty(Defines::JSONKEY_DEVICE_LANGUAGE, language.str());
}

AppInfo&
AppInfo::setPackageName(const String& packageName) {
    return doAddProperty(Defines::JSONKEY_APP_PACKAGE_NAME, packageName.str());
}

std::string
AppInfo::getDeveloperIdentity() const {
    return get(Defines::JSONKEY_APP_DEVELOPER_IDENTITY);
}

AppInfo&
AppInfo::doAddProperty(const char *name, const std::string &value) {
    addProperty(name, value);
    return *this;
}

AppInfo&
AppInfo::doAddProperty(const char *name, int value) {
    addProperty(name, value);
    return *this;
}

}  // namespace BranchIO
