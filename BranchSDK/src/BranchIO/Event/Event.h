// Copyright (c) 2019-21 Branch Metrics, Inc.

#ifndef BRANCHIO_EVENT_EVENT_H_
#define BRANCHIO_EVENT_EVENT_H_

#include <string>
#include "BaseEvent.h"
#include "BranchIO/Defines.h"

namespace BranchIO {

typedef std::string CurrencyType;

/**
 * Base class for Branch events for tracking and analytical purpose.
 */
class BRANCHIO_DLL_EXPORT Event : public BaseEvent {
 public:
    /**
     * Ad Types
     */
    typedef enum tagAdType {
        BANNER = 0,                 ///< Banner Ad Type
        INTERSTITIAL,               ///< Interstitial Ad Type
        REWARDED_VIDEO,             ///< Rewarded Video Ad Type
        NATIVE                      ///< Native Ad Type
    } AdType;

 public:
    /**
     * Copy constructor
     * @param other another instance to copy
     */
    Event(const Event& other);

    virtual ~Event();

    /**
     * Set the Ad Type associated with the event.
     * @param adType {@link AdType} Ad Type value
     * @return this object for chaining builder methods
     */
    virtual Event& setAdType(Event::AdType adType);

    /**
     * Set any affiliation for this transaction event.
     * @param affiliation Any affiliation value
     * @return This object for chaining builder methods
     */
    virtual Event& setAffiliation(const String &affiliation);

    /**
     * Set any coupons associated with this transaction event.
     * @param coupon Any coupon value
     * @return This object for chaining builder methods
     */
    virtual Event& setCoupon(const String &coupon);

    /**
     * Set the currency related with this transaction event.
     * @param currency ISO4217 Code for currency.
     * @return This object for chaining builder methods
     */
    virtual Event& setCurrency(const CurrencyType &currency);

    /**
     * Set the Customer Event Alias with this transaction event.
     * @param alias Customer Event Alias.
     * @return This object for chaining builder methods
     */
    virtual Event& setCustomerEventAlias(const String &alias);

    /**
     * Set description for this transaction event.
     * @param description transaction description
     * @return This object for chaining builder methods
     */
    virtual Event& setDescription(const String &description);

    /**
     * Set the revenue value  related with this transaction event.
     * @param revenue Revenue value
     * @return This object for chaining builder methods
     */
    virtual Event& setRevenue(double revenue);

    /**
     * Set any search query associated with the event.
     * @param searchQuery Search Query value
     * @return This object for chaining builder methods
     */
    virtual Event& setSearchQuery(const String &searchQuery);

    /**
     * Set the shipping value  related with this transaction event.
     * @param shipping Shipping value
     * @return This object for chaining builder methods
     */
    virtual Event& setShipping(double shipping);

    /**
     * Set the tax value  related with this transaction event.
     * @param tax Tax value
     * @return This object for chaining builder methods
     */
    virtual Event& setTax(double tax);

    /**
     * Set the transaction id associated with this event if there in any.
     * @param transactionId Transaction Id
     * @return this object for chaining builder methods
     */
    virtual Event& setTransactionId(const String& transactionId);

    using PropertyManager::toString;

 protected:
    /**
     * Constructor
     * @param apiEndpoint Endpoint this event will be directed towards
     * @param eventName The "Name" of the event
     * @param jsonPtr Base JSON to start with
     */
    Event(Defines::APIEndpoint apiEndpoint, const String& eventName, JSONObject::Ptr jsonPtr = nullptr);

 private:
    Event();

    static const char *stringify(Event::AdType adType);
};

}  // namespace BranchIO

#endif  // BRANCHIO_EVENT_EVENT_H_
