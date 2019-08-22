// Copyright (c) 2019 Branch Metrics, Inc.

#ifndef BRANCHIO_DEFINES_H__
#define BRANCHIO_DEFINES_H__

#include <string>

#include "BranchIO/dll.h"

namespace BranchIO {

/**
 * (Internal) Branch Definitions
 */
class BRANCHIO_DLL_EXPORT Defines {
 public:
    static const char *NO_BRANCH_VALUE;                  ///< No Branch Value

    static const char *JSONKEY_ADTYPE;                   ///< Ad Type
    static const char *JSONKEY_AFFILIATION;              ///< Affiliation
    static const char *JSONKEY_COUPON;                   ///< Coupon Code
    static const char *JSONKEY_CURRENCY;                 ///< ISO4217 Currency Code
    static const char *JSONKEY_DESCRIPTION;              ///< Description
    static const char *JSONKEY_REVENUE;                  ///< Revenue
    static const char *JSONKEY_SEARCHQUERY;              ///< Search Query
    static const char *JSONKEY_SHIPPING;                 ///< Shipping
    static const char *JSONKEY_TAX;                      ///< Tax
    static const char *JSONKEY_TRANSACTION_ID;           ///< Transaction Id

    static const char *JSONKEY_DEVICE_BRAND;             ///< Device Brand
    static const char *JSONKEY_DEVICE_COUNTRY;           ///< Device Country
    static const char *JSONKEY_DEVICE_LANGUAGE;          ///< ISO2 Language
    static const char *JSONKEY_DEVICE_LOCAL_IP_ADDRESS;  ///< Local IP Address
    static const char *JSONKEY_DEVICE_MAC_ADDRESS;       ///< Device MAC Address
    static const char *JSONKEY_DEVICE_MODEL;             ///< Device Model
    static const char *JSONKEY_DEVICE_OS;                ///< Device OS
    static const char *JSONKEY_DEVICE_OS_VERSION;        ///< Device Version
    static const char *JSONKEY_DEVICE_SCREEN_DPI;        ///< Device Screen DPI
    static const char *JSONKEY_DEVICE_SCREEN_HEIGHT;     ///< Device Screen Height (pixels)
    static const char *JSONKEY_DEVICE_SCREEN_WIDTH;      ///< Device Screen Width (pixels)

    static const char *JSONKEY_APP_DEVELOPER_IDENTITY;   ///< App Developer Identity
    static const char *JSONKEY_APP_ENVIRONMENT;          ///< App Environment
    static const char *JSONKEY_APP_LAT_V1;               ///< Limit App Tracking, V1
    static const char *JSONKEY_APP_LAT_V2;               ///< Limit App Tracking, V2
    static const char *JSONKEY_APP_LINK_URL;             ///< App Link Url
    static const char *JSONKEY_APP_PACKAGE_NAME;         ///< App Package Name
    static const char *JSONKEY_APP_SDK;                  ///< SDK
    static const char *JSONKEY_APP_SDK_VERSION;          ///< SDK Version
    static const char *JSONKEY_APP_USER_AGENT;           ///< User Agent
    static const char *JSONKEY_APP_VERSION;              ///< App Version

    static const char *JSONKEY_SESSION_FINGERPRINT;      ///< Session Fingerprint
    static const char *JSONKEY_SESSION_ID;               ///< Session Id
    static const char *JSONKEY_SESSION_IDENTITY;         ///< Session Identity

    // Branch Url Path
    static const char *BASE_PATH_V1;                     ///< V1 Base Path
    static const char *BASE_PATH_V2;                     ///< V2 Base Path

 public:
    /**
     * (Internal) Branch Endpoint Types
     */
    typedef enum tagAPIEndpointType {
        RAW = 0,
        V1,
        V2,
    } APIEndpointType;

    /**
     * (Internal) Branch Endpoints
     */
    typedef enum tagAPIEndpoint {
        // V1 Endpoints
        REGISTER_OPEN,
        REGISTER_CLOSE,
        REGISTER_VIEW,
        LOGOUT,

        // V2 Endpoints
        REDEEM_REWARDS,
        GET_CREDITS,
        GET_CREDIT_HISTORY,
        COMPLETED_ACTION,
        IDENTIFY_USER,
        GET_REFERRAL_CODE,
        VALIDATE_REFERRAL_CODE,
        APPLY_REFERRAL_CODE,
        CONTENT_EVENT,
        TRACK_STANDARD_EVENT,
        TRACK_CUSTOM_EVENT,
    } APIEndpoint;

    /**
     * (Internal) Format an APIEndpoint to a String
     * @param endpoint API Endpoint
     * @return a string representation of the endpoint.
     */
    static const std::string toString(APIEndpoint endpoint);

    /**
     * (Internal) Given an Endpoint, determine if it is a V1 or V2 Type
     * @param endpoint Endpoint
     * @return an EndpointType
     */
    static const APIEndpointType endpointType(APIEndpoint endpoint);
};

}  // namespace BranchIO

#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
#define BRANCH_IO_URL_BASE BranchIO::Defines::BASE_PATH_V2
#else
#define BRANCH_IO_URL_BASE BranchIO::Defines::BASE_PATH_V1
#endif

#endif  // BRANCHIO_DEFINES_H__
