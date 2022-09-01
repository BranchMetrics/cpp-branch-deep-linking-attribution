// Copyright (c) 2019-21 Branch Metrics, Inc.

#ifndef BRANCHIO_DEVICEINFO_H__
#define BRANCHIO_DEVICEINFO_H__

#include <string>
#include "BranchIO/PropertyManager.h"

namespace BranchIO {

/**
 * (Internal) Device Information.
 */
class BRANCHIO_DLL_EXPORT DeviceInfo : public PropertyManager {
 public:
    /**
     * Constructor.
     */
    DeviceInfo();

    /**
     * Constructor.
     * @param jsonObject JSON Object to pre-initialize with
     */
    explicit DeviceInfo(const JSONObject &jsonObject);
    virtual ~DeviceInfo();

    /**
     * (Internal) Set the hardware manufacturer of the current device, as defined by
     * the manufacturer.
     * @param brand Device Brand
     * @return this object for chaining builder methods
     * @todo(andyp): Example
     */
    virtual DeviceInfo& setBrand(const std::string &brand);

    /**
     * (Internal) Set the IP Address of the current device.
     * @param address IP Address
     * @return this object for chaining builder methods
     */
    virtual DeviceInfo& setIPAddress(const std::string &address);

    /**
     * (Internal) Set the MAC Address of the current device.
     * @param address MAC Address
     * @return this object for chaining builder methods
     */
    virtual DeviceInfo& setMACAddress(const std::string &address);

    /**
     * (Internal) Set the hardware model of the current device, as defined by
     * the manufacturer.
     * @param model Model
     * @return This object for chaining builder methods
     */
    virtual DeviceInfo& setModel(const std::string &model);

    /**
     * (Internal) Used by Branch to differentiate between OS types.
     * This is set by the system.  Overriding can lead to attribution errors.
     * @param os OS
     * @return This object for chaining builder methods
     */
     virtual DeviceInfo& setOs(const std::string &os);

    /**
     * (Internal) Used by Branch to differentiate between OS Versions.
     * This is set by the system.  Overriding can lead to attribution errors.
     * @param osVersion OS Version
     * @return This object for chaining builder methods
     */
    virtual DeviceInfo& setOsVersion(const std::string &osVersion);

    /**
    * (Internal) Used by Branch to differentiate between SDKs, e.g. Android, iOS, Windows.
    * This is set by the system.  Overriding can lead to attribution errors.
    * @param sdk SDK Name
    * @return This object for chaining builder methods
    */
    virtual DeviceInfo& setSDK(const std::string &sdk);

    /**
     * (Internal) Used by Branch to differentiate between SDK Versions.
     * This is set by the system.  Overriding can lead to attribution errors.
     * @param sdkVersion SDK Version
     * @return This object for chaining builder methods
     */
    virtual DeviceInfo& setSDKVersion(const std::string &sdkVersion);

    /**
     * (Internal) Set the User Agent.
     * A user agent is a field in a browser request header to help the server understand what type of device the browser is running on.
     * Reference: https://branch.io/glossary/user-agent/
     * @param userAgent User Agent
     * @return This object for chaining builder methods
     */
    virtual DeviceInfo& setUserAgent(const std::string &userAgent);

 private:
    /**
     * Add a string value property to the set.
     * Note that if the value is empty, this effectively removes the key.
     * @param name Key name
     * @param value Key value
     * @return This object for chaining builder methods
     */
    DeviceInfo& doAddProperty(const char *name, const std::string &value);

    /**
     * Add a int value property to the set.
     * @param name Key name
     * @param value Key value
     * @return This object for chaining builder methods
     */
    DeviceInfo& doAddProperty(const char *name, int value);

    /**
     * Initialize the class with known OS values.
     */
    void init();

    /**
     * Determine MAC Address.
     */
    void initMACAddress();

    /**
     *  Determine and the IP address that goes with the default MAC address
     */
    void initIPAddress();

    /**
     *  Determine OS Display Name
     */
    std::string osDisplayName();

    /**
     *  Determine OS Version
     */
    std::string osVersion();
};

}  // namespace BranchIO

#endif  // BRANCHIO_DEVICEINFO_H__
