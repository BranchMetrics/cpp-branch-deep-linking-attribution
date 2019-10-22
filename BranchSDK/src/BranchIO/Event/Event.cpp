// Copyright (c) 2019 Branch Metrics, Inc.

#include "BranchIO/Event/Event.h"
#include "BranchIO/AppInfo.h"
#include "BranchIO/Defines.h"
#include "BranchIO/DeviceInfo.h"
#include "BranchIO/IPackagingInfo.h"
#include "BranchIO/JSONObject.h"
#include "BranchIO/SessionInfo.h"
#include "BranchIO/AdvertiserInfo.h"

namespace BranchIO {

// Top Level JSON Blocks
static const char *JSONKEY_NAME = "name";
static const char *JSONKEY_CUSTOM_DATA = "custom_data";
static const char *JSONKEY_EVENT_DATA = "event_data";
static const char *JSONKEY_USER_DATA = "user_data";
// static const char *JSONKEY_METADATA = "metadata";
static const char *JSONKEY_BRANCH_KEY = "branch_key";
static const char *JSONKEY_ADVERTISING_IDS = "advertising_ids";

// AD TYPES
const char *AD_TYPE_BANER = "BANNER";
const char *AD_TYPE_INTERSTITIAL = "INTERSTITIAL";
const char *AD_TYPE_REWARDED_VIDEO = "REWARDED_VIDEO";
const char *AD_TYPE_NATIVE = "NATIVE";

Event::Event(Defines::APIEndpoint apiEndpoint, const std::string &eventName, JSONObject::Ptr jsonPtr) :
    mAPIEndpoint(apiEndpoint),
    mEventName(eventName),
    mCustomData(NULL) {

    if (jsonPtr.get() != NULL) {
        // Copy the key/values
        for (JSONObject::ConstIterator it = jsonPtr->begin(); it != jsonPtr->end(); ++it) {
            set(it->first, it->second);
        }
    }
}

Event::~Event() { }

Event&
Event::setAdType(Event::AdType adType) {
    return addEventProperty(Defines::JSONKEY_ADTYPE, stringify(adType));
}

Event&
Event::setAffiliation(const std::string &affiliation) {
    return addEventProperty(Defines::JSONKEY_AFFILIATION, affiliation);
}

Event&
Event::setCoupon(const std::string &coupon) {
    return addEventProperty(Defines::JSONKEY_COUPON, coupon);
}

Event&
Event::setCurrency(const CurrencyType &currency) {
    return addEventProperty(Defines::JSONKEY_CURRENCY, currency);
}

Event&
Event::setCustomerEventAlias(const std::string &alias) {
    return addEventProperty(Defines::JSONKEY_CUSTOMER_EVENT_ALIAS, alias);
}

Event&
Event::setDescription(const std::string &description) {
    return addEventProperty(Defines::JSONKEY_DESCRIPTION, description);
}

Event&
Event::setRevenue(double revenue) {
    return addEventProperty(Defines::JSONKEY_REVENUE, revenue);
}

Event&
Event::setSearchQuery(const std::string &searchQuery) {
    return addEventProperty(Defines::JSONKEY_SEARCHQUERY, searchQuery);
}

Event&
Event::setShipping(double shipping)
{
    return addEventProperty(Defines::JSONKEY_SHIPPING, shipping);
}

Event&
Event::setTax(double tax) {
    return addEventProperty(Defines::JSONKEY_TAX, tax);
}

Event&
Event::setTransactionId(const std::string &transactionId) {
    return addEventProperty(Defines::JSONKEY_TRANSACTION_ID, transactionId);
}

Event&
Event::addEventProperty(const char *propertyName, const std::string &propertyValue) {
    addProperty(propertyName, propertyValue);
    return *this;
}

Event&
Event::addEventProperty(const char *propertyName, double propertyValue) {
    addProperty(propertyName, propertyValue);
    return *this;
}

Event&
Event::addCustomDataProperty(
    const std::string &propertyName,
    const std::string &propertyValue) {

    // These go into "custom_data"
    if (mCustomData.isNull()) {
        mCustomData = new JSONObject();
    }

    mCustomData->set(propertyName, propertyValue);
    return *this;
}

const JSONObject::Ptr
Event::getCustomData() const { return mCustomData; }

Defines::APIEndpoint
Event::getAPIEndpoint() const { return mAPIEndpoint; }


const std::string &
Event::name() const { return mEventName; }

const char *
Event::stringify(Event::AdType adType) {
    switch (adType) {
        case BANNER:
            return AD_TYPE_BANER;

        case INTERSTITIAL:
            return AD_TYPE_INTERSTITIAL;

        case REWARDED_VIDEO:
            return AD_TYPE_REWARDED_VIDEO;

        case NATIVE:
            return AD_TYPE_NATIVE;
    }

    return Defines::NO_BRANCH_VALUE;
}


void Event::packageRawEvent(JSONObject &jsonObject) const {
    jsonObject.parse(PropertyManager::stringify());
}

void Event::packageV1Event(IPackagingInfo &packagingInfo, JSONObject &jsonObject) const {
    jsonObject += *this;
    jsonObject += packagingInfo.getSessionInfo().toJSON();
    jsonObject += packagingInfo.getDeviceInfo().toJSON();
    jsonObject += packagingInfo.getAppInfo().toJSON();

    // Ad Tracking Limited
    jsonObject.set(Defines::JSONKEY_APP_LAT_V1, (packagingInfo.getAdvertiserInfo().isTrackingLimited() ? 1 : 0));
}

void Event::packageV2Event(IPackagingInfo &packagingInfo, JSONObject &jsonObject) const {
    // Set the event name
    jsonObject.set(JSONKEY_NAME, name());

    // Set the event data
    jsonObject.set(JSONKEY_EVENT_DATA, *this);

    // Set Custom Data (if any)
    if (!getCustomData().isNull()) {
        jsonObject.set(JSONKEY_CUSTOM_DATA, getCustomData());
    }

    // Set the user data
    JSONObject userData;
    JSONObject sessionInfo = packagingInfo.getSessionInfo().toJSON();
    JSONObject deviceInfo = packagingInfo.getDeviceInfo().toJSON();
    JSONObject appInfo = packagingInfo.getAppInfo().toJSON();
    JSONObject adInfo = packagingInfo.getAdvertiserInfo().toJSON();

    userData += sessionInfo;
    userData += deviceInfo;
    userData += appInfo;

    // Advertising Ids
    bool isAdTrackingLimited = packagingInfo.getAdvertiserInfo().isTrackingLimited();
    if (!isAdTrackingLimited && adInfo.size() > 0) {
        userData.set(JSONKEY_ADVERTISING_IDS, adInfo);
    }
    userData.set(Defines::JSONKEY_APP_LAT_V2, (isAdTrackingLimited ? 1 : 0));

    jsonObject.set(JSONKEY_USER_DATA, userData);
}

void
Event::package(IPackagingInfo &packagingInfo, JSONObject &jsonPackage) const {
    // Set the Branch Key
    jsonPackage.set(JSONKEY_BRANCH_KEY, packagingInfo.getBranchKey());

    switch (Defines::endpointType(getAPIEndpoint())) {
        case Defines::V1:
            packageV1Event(packagingInfo, jsonPackage);
            break;

        case Defines::V2:
            packageV2Event(packagingInfo, jsonPackage);
            break;

        case Defines::RAW:
        default:
            // Dump the raw event data into the request and hope for the best
            packageRawEvent(jsonPackage);
            break;
    }

}


}  // namespace BranchIO
