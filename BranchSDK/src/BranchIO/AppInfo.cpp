// Copyright (c) 2019 Branch Metrics, Inc.

#include "BranchIO/AppInfo.h"

#include "BranchIO/Defines.h"
#include "BranchIO/Internal/JSONObject.h"

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
AppInfo::setAppVersion(const std::string &appVersion) {
    return doAddProperty(Defines::JSONKEY_APP_VERSION, appVersion);
}

AppInfo&
AppInfo::setDeveloperIdentity(const std::string &identity) {
    return doAddProperty(Defines::JSONKEY_APP_DEVELOPER_IDENTITY, identity);
}

AppInfo&
AppInfo::setEnvironment(const std::string &environment) {
    return doAddProperty(Defines::JSONKEY_APP_ENVIRONMENT, environment);
}

AppInfo&
AppInfo::setCountryCode(const std::string &countryCode) {
    return doAddProperty(Defines::JSONKEY_DEVICE_COUNTRY, countryCode);
}

AppInfo&
AppInfo::setDisplayInfo(int dpi, int width, int height) {
    doAddProperty(Defines::JSONKEY_DEVICE_SCREEN_DPI, dpi);
    doAddProperty(Defines::JSONKEY_DEVICE_SCREEN_WIDTH, width);
    doAddProperty(Defines::JSONKEY_DEVICE_SCREEN_HEIGHT, height);

    return *this;
}

AppInfo&
AppInfo::setLanguage(const std::string &language) {
    return doAddProperty(Defines::JSONKEY_DEVICE_LANGUAGE, language);
}

AppInfo&
AppInfo::setPackageName(const std::string &packageName) {
    return doAddProperty(Defines::JSONKEY_APP_PACKAGE_NAME, packageName);
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
