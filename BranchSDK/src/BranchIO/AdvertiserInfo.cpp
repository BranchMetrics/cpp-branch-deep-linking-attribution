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
        case IDFA:                      return "idfa"; break;
        case GOOGLE_ADVERTISING_ID:     return "google_advertising_id"; break;
        case WINDOWS_ADVERTISING_ID:    return "windows_advertising_id"; break;
        case ROKU_RIDA:                 return "roku_rida"; break;
        case SAMSUNG_IFA:               return "samsung_ifa"; break;
        case LG_IFA:                    return "lg_ifa"; break;
        case PANASONIC_IFA:             return "panasonic_ifa"; break;
        case PLAYSTATION_IFA:           return "playstation_ifa"; break;
        case XBOX_MSAI:                 return "xbox_msai"; break;
    }

    return "";
}

}  // namespace BranchIO
