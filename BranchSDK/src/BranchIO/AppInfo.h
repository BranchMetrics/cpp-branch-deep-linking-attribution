// Copyright (c) 2019 Branch Metrics, Inc.

#ifndef BRANCHIO_APPINFO_H__
#define BRANCHIO_APPINFO_H__

#include <string>
#include "BranchIO/PropertyManager.h"

namespace BranchIO {

/**
 * Application Information.
 */
class BRANCHIO_DLL_EXPORT AppInfo : public PropertyManager {
 public:
    /**
     * Constructor.
     */
    AppInfo();

    /**
     * Constructor.
     * @param jsonObject JSON Object to pre-initialize with
     */
    AppInfo(const JSONObject &jsonObject);  // NOLINT
    ~AppInfo();

    /**
     * Set the App Version Name of the current application that the SDK is
     * integrated with.
     * @param appVersion Application Version
     * @return This object for chaining builder methods
     * @todo(andyp): Example
     */
    AppInfo& setAppVersion(const std::string &appVersion);

    /**
     * Set the device country code.
     * @param countryCode A string representing the ISO2 Country code eg US, IN
     * @return This object for chaining builder methods
     * @todo(andyp): Example
     */
    AppInfo& setCountryCode(const std::string &countryCode);

    /**
     * Set the Developer Identity.
     * Unique user id, also known as the Developer Id on your Branch Identity Dashboard
     * Reference: https://docs.branch.io/apps/deep-linking-api/#user-read
     * @param identity Unique User id.  Example: "steve"
     * @return This object for chaining builder methods
     */
    AppInfo& setDeveloperIdentity(const std::string &identity);

    /**
     * Set the Display information.
     * @param dpi Device DPI
     * @param width Device width (in pixels)
     * @param height Device height (in pixels)
     * @return This object for chaining builder methods
     */
    AppInfo& setDisplayInfo(int dpi, int width, int height);

    /**
     * Set the Environment.
     * @param environment Environment
     * @return This object for chaining builder methods
     * @todo(andyp): What is this
     */
    AppInfo& setEnvironment(const std::string &environment);

    /**
     * Set the Device Language.
     * @param language Language
     * @return This object for chaining builder methods
     * @todo(andyp): Example
     */
    AppInfo& setLanguage(const std::string &language);

    /**
     * Set the Package name of the application.
     * @param packageName Package Name
     * @return This object for chaining builder methods
     */
    AppInfo& setPackageName(const std::string &packageName);

    /**
     * Get the developer identity
     * @return the developer identity or an empty string, if none.
     */
    std::string getDeveloperIdentity() const;

 private:
    /**
     * Add a string value property to the set.
     * Note that if the value is empty, this effectively removes the key.
     * @param name Key name
     * @param value Key value
     * @return This object for chaining builder methods
     */
    AppInfo& doAddProperty(const char *name, const std::string &value);

    /**
     * Add a int value property to the set.
     * @param name Key name
     * @param value Key value
     * @return This object for chaining builder methods
     */
    AppInfo& doAddProperty(const char *name, int value);
};

}  // namespace BranchIO

#endif  // BRANCHIO_APPINFO_H__
