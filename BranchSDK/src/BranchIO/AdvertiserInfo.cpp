// Copyright (c) 2019 Branch Metrics, Inc.

#include "BranchIO/AdvertiserInfo.h"
#include "Defines.h"

namespace BranchIO {

AdvertiserInfo::AdvertiserInfo() : PropertyManager(), trackingDisabled(false), trackingLimited(false) {
    trackingDisabled = loadBoolean(AdvertiserStorage, "trackingDisabled", false);
}

AdvertiserInfo::~AdvertiserInfo() = default;

AdvertiserInfo&
AdvertiserInfo::addId(AdIdType type, const std::string &value) {
    PropertyManager::addProperty(stringify(type), value);
    return *this;
}

AdvertiserInfo &
AdvertiserInfo::disableTracking() {
    trackingDisabled = true;
    saveBoolean(AdvertiserStorage, "trackingDisabled", true);
    return *this;
}

AdvertiserInfo &
AdvertiserInfo::enableTracking() {
    trackingDisabled = false;
    saveBoolean(AdvertiserStorage, "trackingDisabled", false);
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
AdvertiserInfo::stringify(AdIdType idType) {
    switch (idType) {
        case IDFA:                      return "idfa";
        case GOOGLE_ADVERTISING_ID:     return "google_advertising_id";
        case WINDOWS_ADVERTISING_ID:    return "windows_advertising_id";
        case ROKU_RIDA:                 return "roku_rida";
        case SAMSUNG_IFA:               return "samsung_ifa";
        case LG_IFA:                    return "lg_ifa";
        case PANASONIC_IFA:             return "panasonic_ifa";
        case PLAYSTATION_IFA:           return "playstation_ifa";
        case XBOX_MSAI:                 return "xbox_msai";
    }

    return "";
}

}  // namespace BranchIO
