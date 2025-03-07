// Copyright (c) 2019-21 Branch Metrics, Inc.

#include "BranchIO/DeviceInfo.h"
#include "BranchIO/Branch.h"
#include "BranchIO/Defines.h"
#include "BranchIO/JSONObject.h"
#include "BranchIO/Util/MacAddress.h"
#include "BranchIO/Util/Log.h"
#include "BranchIO/Util/StringUtils.h"
#include <winsock2.h>
#include <wincrypt.h>
#include <ws2ipdef.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#include <winrt/Windows.Foundation.Metadata.h>

using namespace std;
using namespace winrt::Windows::Foundation::Metadata;

namespace BranchIO {

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
    /*
     * Remove any trailing non-numeric component to generate x.y[.z]
     */
    string::size_type firstNonNumeric(osVersion.find_first_not_of("0123456789."));
    string actualOsVersion(osVersion);
    if (firstNonNumeric != string::npos) {
        actualOsVersion = osVersion.substr(0, firstNonNumeric);
    }

    return doAddProperty(Defines::JSONKEY_DEVICE_OS_VERSION, actualOsVersion);
}

DeviceInfo&
DeviceInfo::setOsBuildNumber(const std::string& osBuild) {
    return doAddProperty(Defines::JSONKEY_DEVICE_OS_BUILD_NUMBER, osBuild);
}

DeviceInfo&
DeviceInfo::setOsPlatformVersion(const std::string& osPlatformVersion) {
    return doAddProperty(Defines::JSONKEY_DEVICE_OS_PLATFORM_VERSION, osPlatformVersion);
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
    setOs(osDisplayName());
    setOsVersion(osVersion());
    setOsBuildNumber(osBuildNumber());
    setOsPlatformVersion(osPlatformVersion());
    setSDK("native");
    setSDKVersion(Branch::getVersion());

    // setBrand()
    // setModel()
    // setUserAgent();

    initIPAndMacAddress();
}

void
DeviceInfo::initIPAndMacAddress() {

    // Note that there are multiple MAC addresses to choose from, however we
    // will use the MAC and IP address associated with the first adapter found.

    // Get all adapter addresses
    IP_ADAPTER_ADDRESSES* adapterAddresses(NULL);
    IP_ADAPTER_ADDRESSES* currAdapterAddresses(NULL);
    ULONG adapterAddressSize = 0;
    DWORD dwRetVal = 0;
    int Iterations = 0;

    // Allocate a 15 KB buffer to start with.
    adapterAddressSize = 15000;

    //Multiple attempts in case interfaces change in the middle of querying them.
    do
    {
        adapterAddresses = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(new char[adapterAddressSize]);

        DWORD dwRetVal = ::GetAdaptersAddresses(
            AF_UNSPEC,
            GAA_FLAG_SKIP_ANYCAST |
            GAA_FLAG_SKIP_MULTICAST |
            GAA_FLAG_SKIP_DNS_SERVER |
            GAA_FLAG_SKIP_FRIENDLY_NAME,
            NULL,
            adapterAddresses,
            &adapterAddressSize);

        if (dwRetVal == ERROR_BUFFER_OVERFLOW)
        {
            delete[] reinterpret_cast<IP_ADAPTER_ADDRESSES*>(adapterAddresses);
            adapterAddresses = NULL;
        }
        else
        {
            break;
        }

        Iterations++;

    } while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < 3));

    if (dwRetVal == NO_ERROR) {
        
        // If successful, fetch MAC and IP
        currAdapterAddresses = adapterAddresses;
        
        bool found = false;

        while (currAdapterAddresses && !found) 
        {
            if (((currAdapterAddresses->IfType == IF_TYPE_ETHERNET_CSMACD) || (currAdapterAddresses->IfType == IF_TYPE_IEEE80211))
                && (currAdapterAddresses->PhysicalAddressLength == 6))
            {
                char macAddress[18] = { 0 };
                sprintf_s(macAddress, (currAdapterAddresses->PhysicalAddressLength * 3),
                    "%02x:%02x:%02x:%02x:%02x:%02x",
                    currAdapterAddresses->PhysicalAddress[0], currAdapterAddresses->PhysicalAddress[1],
                    currAdapterAddresses->PhysicalAddress[2], currAdapterAddresses->PhysicalAddress[3],
                    currAdapterAddresses->PhysicalAddress[4], currAdapterAddresses->PhysicalAddress[5]);
                setMACAddress(std::string(macAddress));

                std::string ipAddress;
                // Parse all IPv4 and IPv6 addresses
                for (
                    IP_ADAPTER_UNICAST_ADDRESS* address = currAdapterAddresses->FirstUnicastAddress;
                    NULL != address;
                    address = address->Next)
                {
                    auto family = address->Address.lpSockaddr->sa_family;
                    if (AF_INET == family)
                    {
                        // IPv4
                        SOCKADDR_IN* ipv4 = reinterpret_cast<SOCKADDR_IN*>(address->Address.lpSockaddr);

                        char str_buffer[INET_ADDRSTRLEN] = { 0 };
                        inet_ntop(AF_INET, &(ipv4->sin_addr), str_buffer, INET_ADDRSTRLEN);

                        ipAddress = string(str_buffer);
                        found = true;
                        break;
                    }
                    else if (AF_INET6 == family)
                    {
                        // IPv6
                        SOCKADDR_IN6* ipv6 = reinterpret_cast<SOCKADDR_IN6*>(address->Address.lpSockaddr);

                        char str_buffer[INET6_ADDRSTRLEN] = { 0 };
                        inet_ntop(AF_INET6, &(ipv6->sin6_addr), str_buffer, INET6_ADDRSTRLEN);

                        std::string ipv6_str(str_buffer);

                        // Detect and skip non-external addresses
                        bool is_link_local(false);
                        bool is_special_use(false);

                        if (0 == ipv6_str.find("fe"))
                        {
                            char c = ipv6_str[2];
                            if (c == '8' || c == '9' || c == 'a' || c == 'b')
                            {
                                is_link_local = true;
                            }
                        }
                        else if (0 == ipv6_str.find("2001:0:"))
                        {
                            is_special_use = true;
                        }

                        if (!(is_link_local || is_special_use))
                        {
                            ipAddress = ipv6_str;
                            found = true;
                            break;
                        }
                    }
                    else
                    {
                        // Skip all other types of addresses
                        continue;
                    }
                }
                if (ipAddress.length() > 0)
                {
                    setIPAddress(ipAddress);
                }
            }
            currAdapterAddresses = currAdapterAddresses->Next;
        }
    }
    else
    {
        // Unexpected error code
        BRANCH_LOG_D("Call to GetAdaptersAddresses failed with error: " << dwRetVal);
        if (dwRetVal == ERROR_NO_DATA)
        {
            BRANCH_LOG_D("No addresses were found for the requested parameters");
        }
        else 
        {
            LPVOID lpMsgBuf = NULL;

            if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, dwRetVal, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR)&lpMsgBuf, 0, NULL)) {

                BRANCH_LOG_D("Error: " << lpMsgBuf);
                LocalFree(lpMsgBuf);

                if (adapterAddresses)
                    delete[] reinterpret_cast<IP_ADAPTER_ADDRESSES*>(adapterAddresses);
                return;
            }
        }
    }
}

std::string DeviceInfo::osDisplayName()
{
    OSVERSIONINFOEX ver;
    ver.dwOSVersionInfoSize = sizeof(ver);
#pragma warning(suppress : 4996)
    if (GetVersionEx((OSVERSIONINFO*)&ver) == 0)
    {
        DWORD error = GetLastError();
        BRANCH_LOG_D("GetVersionEx failed with error: " << error);
    }
    else
    {
        if ((ver.dwMajorVersion == 10) && (ver.dwMinorVersion == 0) && (ver.wProductType == VER_NT_WORKSTATION))
        {
            return  "Windows 10";
        }
        else if ((ver.dwMajorVersion == 10) && (ver.dwMinorVersion == 0) && (ver.wProductType != VER_NT_WORKSTATION))
        {
            return "Windows Server 2016";
        }
        else if ((ver.dwMajorVersion == 6) && (ver.dwMinorVersion == 0) && (ver.wProductType == VER_NT_WORKSTATION))
        {
            return "Windows Vista";
        }
        else if ((ver.dwMajorVersion == 6) && (ver.dwMinorVersion == 0) && (ver.wProductType != VER_NT_WORKSTATION))
        {
            return "Windows Server 2008";
        }
        else if ((ver.dwMajorVersion == 6) && (ver.dwMinorVersion == 1) && (ver.wProductType == VER_NT_WORKSTATION))
        {
            return "Windows 7";
        }
        else if ((ver.dwMajorVersion == 6) && (ver.dwMinorVersion == 1) && (ver.wProductType != VER_NT_WORKSTATION))
        {
            return "Windows Server 2008 R2";
        }
        else if ((ver.dwMajorVersion == 6) && (ver.dwMinorVersion == 2) && (ver.wProductType == VER_NT_WORKSTATION))
        {
            return "Windows 8";
        }
        else if ((ver.dwMajorVersion == 6) && (ver.dwMinorVersion == 2) && (ver.wProductType != VER_NT_WORKSTATION))
        {
            return "Windows Server 2012";
        }
        else if ((ver.dwMajorVersion == 6) && (ver.dwMinorVersion == 3) && (ver.wProductType == VER_NT_WORKSTATION))
        {
            return "Windows 8.1";
        }
        else if ((ver.dwMajorVersion == 6) && (ver.dwMinorVersion == 3) && (ver.wProductType != VER_NT_WORKSTATION))
        {
            return "Windows Server 2012 R2";
        }
        else if ((ver.dwMajorVersion == 5) && (ver.dwMinorVersion == 0) && (ver.wProductType == VER_NT_WORKSTATION))
        {
            return "Windows 2000";
        }
        else if ((ver.dwMajorVersion == 5) && (ver.dwMinorVersion == 1) && (ver.wProductType == VER_NT_WORKSTATION))
        {
            return "Windows XP";
        }
        else if ((ver.dwMajorVersion == 5) && (ver.dwMinorVersion == 2) && (ver.wProductType == VER_NT_WORKSTATION))
        {
            return "Windows Server 2003/Windows Server 2003 R2";
        }
        else
        {
            return "Unknown";
        }
    }
    return "Unknown";
}

bool DeviceInfo::getOSVersionInfo(OSVERSIONINFOEX& ver)
{
    ver.dwOSVersionInfoSize = sizeof(ver);
#pragma warning(suppress : 4996)
    if (GetVersionEx((OSVERSIONINFO*)&ver) == 0)
    {
        DWORD error = GetLastError();
        BRANCH_LOG_D("GetVersionEx failed with error: " << error);
        return false;
    }
    return true;
}

std::string DeviceInfo::osVersion()
{
    OSVERSIONINFOEX ver;
    if (!getOSVersionInfo(ver))
    {
        return "Unknown";
    }

    std::ostringstream osVersionStream;
    osVersionStream << ver.dwMajorVersion << "." << ver.dwMinorVersion;
    return osVersionStream.str();
}

std::string DeviceInfo::osBuildNumber()
{
    OSVERSIONINFOEX ver;
    if (!getOSVersionInfo(ver))
    {
        return "Unknown";
    }

    std::ostringstream buildVersionStream;
    buildVersionStream << (ver.dwBuildNumber & 0xFFFF);
    return buildVersionStream.str();
}

std::string DeviceInfo::osPlatformVersion()
{
        int highestVersion = 0;
        const int MAX_VERSION = 30;  // Max Version - Will fail for Versions after 30. Highest available Major version right now is 19

        // Check all possible versions and return highest one
        for (int version = 1; version <= MAX_VERSION; version++)
        {
            if (ApiInformation::IsApiContractPresent(L"Windows.Foundation.UniversalApiContract", version))
            {
                highestVersion = version; // Keep updating to the highest found version
            }
        }
        BRANCH_LOG_D("Highest available UniversalApiContract version: " << highestVersion << std::endl;);

        return (highestVersion > 0) ? std::to_string(highestVersion) : "Unknown";  
}

}  // namespace BranchIO
