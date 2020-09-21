// Copyright (c) 2019 Branch Metrics, Inc.

#include <string>
#include <sstream>
#include "BranchIO/Defines.h"

namespace BranchIO {

const char *PATH_GET_URL = "v1/url";
const char *PATH_GET_APP = "v1/app-link-settings";
const char *PATH_REGISTER_OPEN = "v1/open";
const char *PATH_REGISTER_CLOSE = "v1/close";
const char *PATH_REGISTER_VIEW = "v1/register-view";
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
        case REGISTER_VIEW:
            ss << PATH_REGISTER_VIEW;
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
        case REGISTER_VIEW:
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
