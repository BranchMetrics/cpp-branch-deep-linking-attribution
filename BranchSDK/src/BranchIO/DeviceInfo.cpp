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


using namespace std;

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
    // will use the address associated with the first adapter found.

    char macAddress[18] = {0};
    int macAddressLen = 6; // Mac Address Length is 6

    PIP_ADAPTER_INFO pAdapterInfo;
    ULONG ulOutBufLen = 0;
    DWORD dwRetVal = 0;

    // Call GetAdaptersInfo first to get the len for allocating pAdapterInfo
    DWORD rc = GetAdaptersInfo(NULL, &ulOutBufLen);
    if (rc == ERROR_BUFFER_OVERFLOW)
    {
        pAdapterInfo = reinterpret_cast<IP_ADAPTER_INFO*>(new char[ulOutBufLen]);

        if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
        {
            PIP_ADAPTER_INFO tempAdapterInfo = pAdapterInfo;
            bool found = false;
            while (tempAdapterInfo && !found)
            {
                if (tempAdapterInfo->Type == MIB_IF_TYPE_ETHERNET && tempAdapterInfo->AddressLength == macAddressLen)
                {
                    found = true;
                    sprintf(macAddress, "%02x:%02x:%02x:%02x:%02x:%02x",
                        pAdapterInfo->Address[0], pAdapterInfo->Address[1],
                        pAdapterInfo->Address[2], pAdapterInfo->Address[3],
                        pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
                }
                tempAdapterInfo = tempAdapterInfo->Next;

            }

            if(found)
                setMACAddress(std::string(macAddress));
        }
        else
        {
            BRANCH_LOG_D("GetAdaptersInfo failed with error: " << dwRetVal);
        }
        delete[] reinterpret_cast<char*>(pAdapterInfo);
    }
    else
    {
        BRANCH_LOG_D("Couldn't get length for buffer. GetAdaptersInfo failed with error: " << dwRetVal);
    }
}

void
DeviceInfo::initIPAddress() {

    // Note that there are multiple IP addresses to choose from, however we
    // will use the address associated with the current MAC address

    bool found = false;
    std::string ipAddress;

    string defaultMacAddress = getStringProperty(Defines::JSONKEY_DEVICE_MAC_ADDRESS);

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
        
        // If successful, fetch IP
        currAdapterAddresses = adapterAddresses;
        
        bool found = false;

        while (currAdapterAddresses && !found) 
        {
            // Skip loopback adapters
            if (IF_TYPE_SOFTWARE_LOOPBACK == currAdapterAddresses->IfType)
            {
                continue;
            }

            // Compare Mac address - Skip if MAC address is not equal to default Mac Address.
            if (!defaultMacAddress.compare(std::string(currAdapterAddresses->PhysicalAddress, currAdapterAddresses->PhysicalAddress + currAdapterAddresses->PhysicalAddressLength)))
            {
                continue;
            }

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

            currAdapterAddresses = currAdapterAddresses->Next;
        }
    }
    else
    {
        // Unexpected error code
        adapterAddresses = NULL;
        BRANCH_LOG_D("Call to GetAdaptersAddresses failed with error: " << dwRetVal);
        if (dwRetVal == ERROR_NO_DATA)
            printf("No addresses were found for the requested parameters\n");
        else {

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
  
    if (ipAddress.length() > 0) 
    {
        setIPAddress(ipAddress);
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


std::string DeviceInfo::osVersion()
{
    std::string osVersion;
    std::ostringstream osVersionStream;
    
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
        osVersionStream << ver.dwMajorVersion << "." << ver.dwMinorVersion << " (Build " << (ver.dwBuildNumber & 0xFFFF);
        wstring ws(ver.szCSDVersion);
        string csdVersion(StringUtils::wstring_to_utf8(ws));
        if (!csdVersion.empty()) osVersionStream << ": " << csdVersion;
        osVersionStream << ")";
    }
   
    return osVersionStream.str();
}


}  // namespace BranchIO
