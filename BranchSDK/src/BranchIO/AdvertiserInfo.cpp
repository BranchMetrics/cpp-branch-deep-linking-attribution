// Copyright (c) 2019 Branch Metrics, Inc.

#include "BranchIO/AdvertiserInfo.h"
#include "BranchIO/Defines.h"
#include "BranchIO/Util/Storage.h"

namespace BranchIO {

static const char *const ADVERTISERSTORAGE = "advertiser";
static const char *const TRACKING_PREFERENCE_KEY = "trackingDisabled";

AdvertiserInfo::AdvertiserInfo() : PropertyManager(), trackingDisabled(false), trackingLimited(false) {
    trackingDisabled =
            Storage::instance().getBoolean(getPath(ADVERTISERSTORAGE, TRACKING_PREFERENCE_KEY));
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
    Storage::instance().setBoolean(getPath(ADVERTISERSTORAGE, TRACKING_PREFERENCE_KEY), true);
    return *this;
}

AdvertiserInfo &
AdvertiserInfo::enableTracking() {
    trackingDisabled = false;
    Storage::instance().setBoolean(getPath(ADVERTISERSTORAGE, TRACKING_PREFERENCE_KEY), false);
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
