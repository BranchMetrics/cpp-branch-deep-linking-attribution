// Copyright (c) 2019 Branch Metrics, Inc.

#ifndef BRANCHIO_EVENT_EVENT_H_
#define BRANCHIO_EVENT_EVENT_H_

#include <string>
#include "BranchIO/Defines.h"
#include "BranchIO/fwd.h"
#include "BranchIO/PropertyManager.h"

namespace BranchIO {

typedef std::string CurrencyType;

/**
 * Base class for Branch events for tracking and analytical purpose.
 */
class BRANCHIO_DLL_EXPORT Event : public PropertyManager {
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
    virtual Event& setAffiliation(const std::string &affiliation);

    /**
     * Set any coupons associated with this transaction event.
     * @param coupon Any coupon value
     * @return This object for chaining builder methods
     */
    virtual Event& setCoupon(const std::string &coupon);

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
    virtual Event& setCustomerEventAlias(const std::string &alias);

    /**
     * Set description for this transaction event.
     * @param description transaction description
     * @return This object for chaining builder methods
     */
    virtual Event& setDescription(const std::string &description);

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
    virtual Event& setSearchQuery(const std::string &searchQuery);

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
    virtual Event& setTransactionId(const std::string &transactionId);

    /**
     * Adds a custom data property associated with this Branch Event.
     * @param propertyName Name of the custom property
     * @param propertyValue Value of the custom property
     * @return This object for chaining builder methods
     */
    virtual Event& addCustomDataProperty(
        const std::string &propertyName,
        const std::string &propertyValue);

    /**
     * Get the custom data properties associated with this Branch Event.
     * @return the Custom Data for the event.
     */
    virtual const JSONObject::Ptr getCustomData() const;

    /**
     * @return the Event Name.
     */
    virtual const std::string &name() const;

    using PropertyManager::toString;

 public:
    /**
     * @return the API Endpoint.
     */
    virtual Defines::APIEndpoint getAPIEndpoint() const;

    /**
     * Prepare the event package for transmission.
     * @param packagingInfo Context for packaging events
     * @param jsonPackage result of the packaging process
     * @todo(andyp) Revisit Scope
     */
    virtual void package(IPackagingInfo &packagingInfo, JSONObject &jsonPackage) const;

 protected:
    /**
     * Constructor
     * @param apiEndpoint Endpoint this event will be directed towards
     * @param eventName The "Name" of the event
     * @param jsonPtr Base JSON to start with
     */
    Event(Defines::APIEndpoint apiEndpoint, const std::string &eventName, JSONObject::Ptr jsonPtr = NULL);

    /**
     * Add a Raw Event Property Name and Value
     * @param propertyName Property Name
     * @param propertyValue Property Value
     * @return this object for chaining builder methods
     */
    Event& addEventProperty(const char *propertyName, const std::string &propertyValue);

    /**
     * Add a Raw Event Property Name and Value
     * @param propertyName Property Name
     * @param propertyValue Property Value
     * @return this object for chaining builder methods
     */
    Event& addEventProperty(const char *propertyName, double propertyValue);

 private:
    Event();

    static const char *stringify(Event::AdType adType);

    void packageRawEvent(JSONObject &jsonObject) const;
    void packageV1Event(IPackagingInfo &branch, JSONObject &jsonObject) const;
    void packageV2Event(IPackagingInfo &branch, JSONObject &jsonObject) const;

 private:
    // API Endpoint
    Defines::APIEndpoint mAPIEndpoint;

    // The Event Name
    std::string mEventName;

    // Custom Data is rendered as a sibling to the event data
    JSONObject::Ptr mCustomData;
};

}  // namespace BranchIO

#endif  // BRANCHIO_EVENT_EVENT_H_
