// Copyright (c) 2019 Branch Metrics, Inc.

#include "Event.h"
#include "BranchIO/Defines.h"
#include "BranchIO/JSONObject.h"
#include "BranchIO/String.h"

namespace BranchIO {

// AD TYPES
const char *AD_TYPE_BANER = "BANNER";
const char *AD_TYPE_INTERSTITIAL = "INTERSTITIAL";
const char *AD_TYPE_REWARDED_VIDEO = "REWARDED_VIDEO";
const char *AD_TYPE_NATIVE = "NATIVE";

Event::Event(Defines::APIEndpoint apiEndpoint, const String& eventName, JSONObject::Ptr jsonPtr) :
    BaseEvent(apiEndpoint, eventName, jsonPtr) {
}

Event::Event(const Event& other)
    : BaseEvent(other) {
}

Event::~Event() = default;

Event&
Event::setAdType(Event::AdType adType) {
    addEventProperty(Defines::JSONKEY_ADTYPE, stringify(adType));
    return *this;
}

Event&
Event::setAffiliation(const String& affiliation) {
    addEventProperty(Defines::JSONKEY_AFFILIATION, affiliation);
    return *this;
}

Event&
Event::setCoupon(const String& coupon) {
    addEventProperty(Defines::JSONKEY_COUPON, coupon);
    return *this;
}

Event&
Event::setCurrency(const CurrencyType &currency) {
    addEventProperty(Defines::JSONKEY_CURRENCY, currency);
    return *this;
}

Event&
Event::setCustomerEventAlias(const String& alias) {
    addEventProperty(Defines::JSONKEY_CUSTOMER_EVENT_ALIAS, alias);
    return *this;
}

Event&
Event::setDescription(const String& description) {
    addEventProperty(Defines::JSONKEY_DESCRIPTION, description);
    return *this;
}

Event&
Event::setRevenue(double revenue) {
    addEventProperty(Defines::JSONKEY_REVENUE, revenue);
    return *this;
}

Event&
Event::setSearchQuery(const String& searchQuery) {
    addEventProperty(Defines::JSONKEY_SEARCHQUERY, searchQuery);
    return *this;
}

Event&
Event::setShipping(double shipping) {
    addEventProperty(Defines::JSONKEY_SHIPPING, shipping);
    return *this;
}

Event&
Event::setTax(double tax) {
    addEventProperty(Defines::JSONKEY_TAX, tax);
    return *this;
}

Event&
Event::setTransactionId(const String& transactionId) {
    addEventProperty(Defines::JSONKEY_TRANSACTION_ID, transactionId);
    return *this;
}

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

}  // namespace BranchIO
