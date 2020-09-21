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
    static constexpr const char* const NO_BRANCH_VALUE = "";                                   ///< No Branch Value

    static constexpr const char* const JSONKEY_ADTYPE = "ad_type";                             ///< Ad Type
    static constexpr const char* const JSONKEY_AFFILIATION = "ad_type";                        ///< Affiliation
    static constexpr const char* const JSONKEY_COUPON = "coupon";                              ///< Coupon Code
    static constexpr const char* const JSONKEY_CURRENCY = "currency";                          ///< ISO4217 Currency Code
    static constexpr const char* const JSONKEY_CUSTOMER_EVENT_ALIAS = "customer_event_alias";  ///< Customer Event Alias
    static constexpr const char* const JSONKEY_DESCRIPTION = "description";                    ///< Description
    static constexpr const char* const JSONKEY_REVENUE = "revenue";                            ///< Revenue
    static constexpr const char* const JSONKEY_SEARCHQUERY = "search_query";                   ///< Search Query
    static constexpr const char* const JSONKEY_SHIPPING = "shipping";                          ///< Shipping
    static constexpr const char* const JSONKEY_TAX = "tax";                                    ///< Tax
    static constexpr const char* const JSONKEY_TRANSACTION_ID = "transaction_id";              ///< Transaction Id

    static constexpr const char* const JSONKEY_DEVICE_BRAND = "brand";                         ///< Device Brand
    static constexpr const char* const JSONKEY_DEVICE_COUNTRY = "country";                     ///< Device Country
    static constexpr const char* const JSONKEY_DEVICE_LANGUAGE = "language";                   ///< ISO2 Language
    static constexpr const char* const JSONKEY_DEVICE_LOCAL_IP_ADDRESS = "local_ip";           ///< Local IP Address
    static constexpr const char* const JSONKEY_DEVICE_MAC_ADDRESS = "mac_address";             ///< Device MAC Address
    static constexpr const char* const JSONKEY_DEVICE_MODEL = "model";                         ///< Device Model
    static constexpr const char* const JSONKEY_DEVICE_OS = "os";                               ///< Device OS
    static constexpr const char* const JSONKEY_DEVICE_OS_VERSION = "os_version";               ///< Device Version
    static constexpr const char* const JSONKEY_DEVICE_SCREEN_DPI = "screen_dpi";               ///< Device Screen DPI
    static constexpr const char* const JSONKEY_DEVICE_SCREEN_HEIGHT = "screen_height";         ///< Device Screen Height (pixels)
    static constexpr const char* const JSONKEY_DEVICE_SCREEN_WIDTH = "screen_width";           ///< Device Screen Width (pixels)

    static constexpr const char* const JSONKEY_APP_IDENTITY = "identity";                      ///< Developer Identity, V1
    static constexpr const char* const JSONKEY_APP_DEVELOPER_IDENTITY = "developer_identity";  ///< Developer Identity, V2
    static constexpr const char* const JSONKEY_APP_ENVIRONMENT = "environment";                ///< App Environment
    static constexpr const char* const JSONKEY_APP_LAT_V1 = "lat_val";                         ///< Limit App Tracking, V1
    static constexpr const char* const JSONKEY_APP_LAT_V2 = "limit_ad_tracking";               ///< Limit App Tracking, V2
    static constexpr const char* const JSONKEY_APP_LINK_URL = "app_link_url";                  ///< App Link Url
    static constexpr const char* const JSONKEY_APP_PACKAGE_NAME = "package_name";              ///< App Package Name
    static constexpr const char* const JSONKEY_APP_SDK = "sdk";                                ///< SDK
    static constexpr const char* const JSONKEY_APP_SDK_VERSION = "sdk_version";                ///< SDK Version
    static constexpr const char* const JSONKEY_APP_USER_AGENT = "user_agent";                  ///< User Agent
    static constexpr const char* const JSONKEY_APP_VERSION = "app_version";                    ///< App Version

    static constexpr const char* const JSONKEY_SESSION_FINGERPRINT = "device_fingerprint_id";  ///< Device Fingerprint ID
    static constexpr const char* const JSONKEY_SESSION_ID = "session_id";                      ///< Session Id
    static constexpr const char* const JSONKEY_SESSION_IDENTITY = "identity_id";               ///< Session Identity
    static constexpr const char* const JSONKEY_TRACKING_DISABLED = "tracking_disabled";        ///< Tracking Disabled

    // Branch Url Path
    static constexpr const char* const BASE_PATH_V1 = "https://api.branch.io/";                ///< V1 Base Path
    static constexpr const char* const BASE_PATH_V2 = "https://api2.branch.io/";               ///< V2 Base Path

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
        URL,

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
    static const std::string stringify(APIEndpoint endpoint);

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
