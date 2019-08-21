// Copyright (c) 2019 Branch Metrics, Inc.

#include "BranchIO/DeviceInfo.h"

#include <Poco/Environment.h>
#include <Poco/Net/NetworkInterface.h>

#include "BranchIO/Branch.h"
#include "BranchIO/Defines.h"
#include "BranchIO/JSONObject.h"
#include "BranchIO/MacAddress.h"

using Poco::Net::NetworkInterface;
using Poco::Net::IPAddress;

namespace BranchIO {

std::string getOsName();

DeviceInfo::DeviceInfo() : PropertyManager() {
    init();
}

DeviceInfo::DeviceInfo(const JSONObject &jsonObject)
    : PropertyManager(jsonObject) {
}

DeviceInfo::~DeviceInfo() {
}

DeviceInfo&
DeviceInfo::setBrand(const std::string &brand) {
    return doAddProperty(Defines::JSONKEY_DEVICE_BRAND, brand);
}

DeviceInfo&
DeviceInfo::setIPAddress(const std::string &address) {
    return doAddProperty(Defines::JSONKEY_DEVICE_LOCAL_IP_ADDRESS, address);
}

DeviceInfo&
DeviceInfo::setMACAddress(const std::string &address) {
    return doAddProperty(Defines::JSONKEY_DEVICE_MAC_ADDRESS, address);
}

DeviceInfo&
DeviceInfo::setModel(const std::string &model) {
    return doAddProperty(Defines::JSONKEY_DEVICE_MODEL, model);
}

DeviceInfo&
DeviceInfo::setOs(const std::string &os) {
    return doAddProperty(Defines::JSONKEY_DEVICE_OS, os);
}

DeviceInfo&
DeviceInfo::setOsVersion(const std::string &osVersion) {
    return doAddProperty(Defines::JSONKEY_DEVICE_OS_VERSION, osVersion);
}

DeviceInfo&
DeviceInfo::setSDK(const std::string &sdk) {
    return doAddProperty(Defines::JSONKEY_APP_SDK, sdk);
}

DeviceInfo&
DeviceInfo::setSDKVersion(const std::string &sdkVersion) {
    return doAddProperty(Defines::JSONKEY_APP_SDK_VERSION, sdkVersion);
}

DeviceInfo&
DeviceInfo::setUserAgent(const std::string &userAgent) {
    return doAddProperty(Defines::JSONKEY_APP_USER_AGENT, userAgent);
}

DeviceInfo&
DeviceInfo::doAddProperty(const char *name, const std::string &value) {
    addProperty(name, value);
    return *this;
}

DeviceInfo&
DeviceInfo::doAddProperty(const char *name, int value) {
    addProperty(name, value);
    return *this;
}

void
DeviceInfo::init() {
    setOs(Poco::Environment::osDisplayName());
    setOsVersion(Poco::Environment::osVersion());
    setSDK("native");
    setSDKVersion(Branch::getVersion());

    // setBrand()
    // setModel()
    // setUserAgent();

    initMACAddress();
    initIPAddress();
}

void
DeviceInfo::initMACAddress() {
    // Note that there are multiple MAC addresses to choose from, however we
    // will use the address associated with the current "nodeId", which corresponds
    // to the first adapter found.
    try {
        std::string nodeId = Poco::Environment::nodeId();
        setMACAddress(nodeId);
    } catch (const Poco::SystemException &e) {
        // TODO(andyp): Log
    }
}

void
DeviceInfo::initIPAddress() {
    // Note that there are multiple IP addresses to choose from, however we
    // will use the address associated with the current MAC address
    bool found = false;
    std::string ip_address;

    NetworkInterface::Map m = NetworkInterface::map(false, false);
    if (m.empty()) {
        return;
    }

    // TODO(andyp): Get the MAC Address from the property manager
    MacAddress defaultMacAddress = Poco::Environment::nodeId();

    // Iterate through all of the network interfaces looking for the default MAC address
    for (NetworkInterface::Map::const_iterator it = m.begin(); it != m.end() && !found; ++it) {
        const NetworkInterface& iface(it->second);
        if (!iface.isUp()) {
            continue;
        }

        MacAddress mac(iface.macAddress());
        if (mac.empty()) {
            continue;
        }

        /* Probably never hit, since loopback interfaces don't have a MAC
         * address, but good validation that doesn't really cost anything.
         */
        if (iface.type() == NetworkInterface::NI_TYPE_SOFTWARE_LOOPBACK) {
            continue;
        }

        if (!mac.valid()) {
            continue;
        }

        if (mac != defaultMacAddress) {
            continue;
        }

        typedef NetworkInterface::AddressList List;

        const List& ipList = iface.addressList();
        List::const_iterator ipIt = ipList.begin();
        List::const_iterator ipEnd = ipList.end();
        for (int counter = 0; ipIt != ipEnd; ++ipIt, ++counter) {
            ip_address = ipIt->get<NetworkInterface::IP_ADDRESS>().toString();
            if (ip_address.length() > 0) {
                // Found it!  Let's get out of here.
                found = true;
                break;
            }
        }
    }

    if (ip_address.length() > 0) {
        setIPAddress(ip_address);
    }
}

std::string
getOsName() {
    std::stringstream ss;

    switch (Poco::Environment::os()) {
        case POCO_OS_FREE_BSD:
        case POCO_OS_AIX:
        case POCO_OS_HPUX:
        case POCO_OS_TRU64:
        case POCO_OS_NET_BSD:
        case POCO_OS_OPEN_BSD:
        case POCO_OS_IRIX:
        case POCO_OS_SOLARIS:
        case POCO_OS_QNX:
        case POCO_OS_UNKNOWN_UNIX:
            // Unix
            ss << "Unix";
            break;

        case POCO_OS_LINUX:
            // Linux
            ss << "Linux";
            break;

        case POCO_OS_MAC_OS_X:
            // Mac
            ss << "MacOS";
            break;

        case POCO_OS_CYGWIN:
        case POCO_OS_WINDOWS_NT:
        case POCO_OS_WINDOWS_CE:
            // Windows
            ss << "Windows";
            break;

        case POCO_OS_NACL:
            // Google Native Client
            ss << "Google";
            break;

        case POCO_OS_ANDROID:
            // Android
            ss << "Android";
            break;

        case POCO_OS_VXWORKS:
            // Real Time
            ss << "VXWorks";
            break;

        case POCO_OS_VMS:
            // VMS
            ss << "VMS";
            break;

        default:
            ss << "UnknownOS";
            break;
    }

    return ss.str();
}

}  // namespace BranchIO
