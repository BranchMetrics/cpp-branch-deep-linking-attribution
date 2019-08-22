// Copyright (c) 2019 Branch Metrics, Inc.

#include "BranchIO/AdvertiserInfo.h"
#include "Defines.h"

namespace BranchIO {

AdvertiserInfo::AdvertiserInfo() : PropertyManager(), trackingDisabled(false), trackingLimited(false) {
}

AdvertiserInfo::~AdvertiserInfo() {
}

AdvertiserInfo&
AdvertiserInfo::addId(AdIdType type, const std::string &value) {
    const char *key = toXString(type);

    PropertyManager::addProperty(key, value);
    return *this;
}

AdvertiserInfo &
AdvertiserInfo::disableTracking() {
    trackingDisabled = true;
    return *this;
}

AdvertiserInfo &
AdvertiserInfo::enableTracking() {
    trackingDisabled = false;
    return *this;
}

AdvertiserInfo &
AdvertiserInfo::limitAdTracking(bool is_lat) {
    trackingLimited = is_lat;
    return *this;
}

bool
AdvertiserInfo::isTrackingDisabled() const {
    return trackingDisabled;
}

bool
AdvertiserInfo::isTrackingLimited() const {
    return trackingLimited;
}

const char *
AdvertiserInfo::toXString(AdIdType idType) const {
    switch (idType) {
        case IDFA:                      return "IDFA"; break;
        case GOOGLE_ADVERTISING_ID:     return "GOOGLE_ADVERTISING_ID"; break;
        case WINDOWS_ADVERTISING_ID:    return "WINDOWS_ADVERTISING_ID"; break;
        case ROKU_RIDA:                 return "ROKU_RIDA"; break;
        case SAMSUNG_IFA:               return "SAMSUNG_IFA"; break;
        case LG_IFA:                    return "LG_IFA"; break;
        case PANASONIC_IFA:             return "PANASONIC_IFA"; break;
        case PLAYSTATION_IFA:           return "PLAYSTATION_IFA"; break;
        case XBOX_MSAI:                 return "XBOX_MSAI"; break;
    }

    return "";
}

}  // namespace BranchIO
