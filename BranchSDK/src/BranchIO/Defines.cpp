// Copyright (c) 2019-21 Branch Metrics, Inc.

#include <string>
#include <sstream>
#include "BranchIO/Defines.h"

namespace BranchIO {

// GENERAL CONSTANTS
const char *Defines::NO_BRANCH_VALUE = "";

// JSON KEYS
const char *Defines::JSONKEY_ADTYPE = "ad_type";
const char *Defines::JSONKEY_AFFILIATION = "affiliation";
const char *Defines::JSONKEY_COUPON = "coupon";
const char *Defines::JSONKEY_CURRENCY = "currency";
const char *Defines::JSONKEY_CUSTOMER_EVENT_ALIAS = "customer_event_alias";
const char *Defines::JSONKEY_DESCRIPTION = "description";
const char *Defines::JSONKEY_REVENUE = "revenue";
const char *Defines::JSONKEY_SEARCHQUERY = "search_query";
const char *Defines::JSONKEY_SHIPPING = "shipping";
const char *Defines::JSONKEY_TAX = "tax";
const char *Defines::JSONKEY_TRANSACTION_ID = "transaction_id";

const char *Defines::JSONKEY_DEVICE_BRAND = "brand";
const char *Defines::JSONKEY_DEVICE_COUNTRY = "country";
const char *Defines::JSONKEY_DEVICE_LANGUAGE = "language";
const char *Defines::JSONKEY_DEVICE_LOCAL_IP_ADDRESS = "local_ip";
const char *Defines::JSONKEY_DEVICE_MAC_ADDRESS = "mac_address";
const char *Defines::JSONKEY_DEVICE_MODEL = "model";
const char *Defines::JSONKEY_DEVICE_OS = "os";
const char *Defines::JSONKEY_DEVICE_OS_VERSION = "os_version";
const char *Defines::JSONKEY_DEVICE_SCREEN_DPI = "screen_dpi";
const char *Defines::JSONKEY_DEVICE_SCREEN_HEIGHT = "screen_height";
const char *Defines::JSONKEY_DEVICE_SCREEN_WIDTH = "screen_width";

const char *Defines::JSONKEY_APP_IDENTITY = "identity";
const char *Defines::JSONKEY_APP_DEVELOPER_IDENTITY = "developer_identity";
const char *Defines::JSONKEY_APP_ENVIRONMENT = "environment";
const char *Defines::JSONKEY_APP_LAT_V1 = "lat_val";
const char *Defines::JSONKEY_APP_LAT_V2 = "limit_ad_tracking";
const char *Defines::JSONKEY_APP_LINK_URL = "app_link_url";
const char *Defines::JSONKEY_APP_PACKAGE_NAME = "package_name";
const char *Defines::JSONKEY_APP_SDK = "sdk";
const char *Defines::JSONKEY_APP_SDK_VERSION = "sdk_version";
const char *Defines::JSONKEY_APP_USER_AGENT = "user_agent";
const char *Defines::JSONKEY_APP_VERSION = "app_version";

const char *Defines::JSONKEY_SESSION_FINGERPRINT = "device_fingerprint_id";
const char* Defines::JSONKEY_SESSION_RANDOMIZED_DEVICE_TOKEN = "randomized_device_token";
const char *Defines::JSONKEY_SESSION_ID = "session_id";
const char *Defines::JSONKEY_SESSION_IDENTITY = "identity_id";
const char* Defines::JSONKEY_SESSION_RANDOMIZED_BUNDLE_TOKEN = "randomized_bundle_token";
const char *Defines::JSONKEY_TRACKING_DISABLED = "tracking_disabled";
const char* Defines::JSONKEY_WINDOWS_ADVERTISING_ID = "windows_advertising_id";
const char* Defines::JSONKEY_LINK_IDENTIFIER = "link_identifier";
const char* Defines::JSONKEY_LINK = "link";
// Branch Url Path
const char *Defines::BASE_PATH_V2 = "https://api2.branch.io/";

const char *PATH_GET_URL = "v1/url";
const char *PATH_GET_APP = "v1/app-link-settings";
const char *PATH_REGISTER_OPEN = "v1/open";
const char *PATH_REGISTER_CLOSE = "v1/close";
const char *PATH_LOGOUT = "v1/logout";
const char *PATH_URL = "v1/url";

const char *PATH_REDEEM_REWARDS = "v1/redeem";
const char *PATH_GET_CREDITS = "v1/credits";
const char *PATH_GET_CREDIT_HISTORY = "v1/credithistory";
const char *PATH_COMPLETED_ACTION = "v1/event";
const char *PATH_IDENTIFY_USER = "v1/profile";
const char *PATH_GET_REFERRAL_CODE = "v1/referralcode";
const char *PATH_VALIDATE_REFERRAL_CODE = "v1/referralcode";
const char *PATH_APPLY_REFERRAL_CODE = "v1/applycode";
const char *PATH_CONTENT_EVENT = "v1/content-events";
const char *PATH_TRACK_STANDARD_EVENT = "v2/event/standard";
const char *PATH_TRACK_CUSTOM_EVENT = "v2/event/custom";

const std::string
Defines::stringify(APIEndpoint apiEndpoint) {
    std::stringstream ss;
    ss << BRANCH_IO_URL_BASE;

    switch (apiEndpoint) {
        case REGISTER_OPEN:
            ss << PATH_REGISTER_OPEN;
            break;
        case REGISTER_CLOSE:
            ss << PATH_REGISTER_CLOSE;
            break;
        case LOGOUT:
            ss << PATH_LOGOUT;
            break;
        case URL:
            ss << PATH_URL;
            break;

        case APPLY_REFERRAL_CODE:
            ss << PATH_APPLY_REFERRAL_CODE;
            break;
        case COMPLETED_ACTION:
            ss << PATH_COMPLETED_ACTION;
            break;
        case CONTENT_EVENT:
            ss << PATH_CONTENT_EVENT;
            break;
        case GET_CREDITS:
            ss << PATH_GET_CREDITS;
            break;
        case GET_CREDIT_HISTORY:
            ss << PATH_GET_CREDIT_HISTORY;
            break;
        case GET_REFERRAL_CODE:
            ss << PATH_GET_REFERRAL_CODE;
            break;
        case IDENTIFY_USER:
            ss << PATH_IDENTIFY_USER;
            break;
        case REDEEM_REWARDS:
            ss << PATH_REDEEM_REWARDS;
            break;
        case TRACK_CUSTOM_EVENT:
            ss << PATH_TRACK_CUSTOM_EVENT;
            break;
        case TRACK_STANDARD_EVENT:
            ss << PATH_TRACK_STANDARD_EVENT;
            break;
        case VALIDATE_REFERRAL_CODE:
            ss << PATH_VALIDATE_REFERRAL_CODE;
            break;

        default:
            // @todo(andyp): Error Handling
            break;
    }

    // Create the URI from the string
    return ss.str();
}

const Defines::APIEndpointType
Defines::endpointType(APIEndpoint apiEndpoint) {
    APIEndpointType apiType = RAW;
    switch (apiEndpoint) {
        case REGISTER_OPEN:
        case REGISTER_CLOSE:
        case IDENTIFY_USER:
        case LOGOUT:
        case URL:
            apiType = V1;
            break;

        case APPLY_REFERRAL_CODE:
        case COMPLETED_ACTION:
        case CONTENT_EVENT:
        case GET_CREDITS:
        case GET_CREDIT_HISTORY:
        case GET_REFERRAL_CODE:
        case REDEEM_REWARDS:
        case TRACK_CUSTOM_EVENT:
        case TRACK_STANDARD_EVENT:
        case VALIDATE_REFERRAL_CODE:
            apiType = V2;
            break;

        default:
            break;
    }

    return apiType;
}

}  // namespace BranchIO
