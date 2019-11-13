// Copyright (c) 2019 Branch Metrics, Inc.

#ifndef BRANCHIO_EVENT_BASEEVENT_H_
#define BRANCHIO_EVENT_BASEEVENT_H_

#include <string>
#include "BranchIO/Defines.h"
#include "BranchIO/fwd.h"
#include "BranchIO/PropertyManager.h"

namespace BranchIO {

typedef std::string CurrencyType;

/**
 * Base class for Branch events for tracking and analytical purpose.
 */
class BRANCHIO_DLL_EXPORT BaseEvent : public PropertyManager {
 public:
    using PropertyManager::toString;

    /**
     * Copy constructor
     * @param other another instance to copy
     */
    BaseEvent(const BaseEvent& other);

    virtual ~BaseEvent();

    /**
     * @return the Event Name.
     */
    virtual const std::string &name() const;

    /**
     * Adds a custom data property associated with this Branch Event.
     * Note that these properties are protocol-specific, and may not be sent in all cases.
     * @param propertyName Name of the custom property
     * @param propertyValue Value of the custom property
     * @return This object for chaining builder methods
     */
    virtual BaseEvent& addCustomDataProperty(const std::string &propertyName, const std::string &propertyValue);

    /**
     * Get the custom data properties associated with this Branch Event.
     * @return the Custom Data for the event.
     */
    virtual const JSONObject::Ptr getCustomData() const;

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
    BaseEvent(Defines::APIEndpoint apiEndpoint, const std::string &eventName, JSONObject::Ptr jsonPtr = nullptr);

    /**
     * Add a Raw Event Property Name and Value
     * @param propertyName Property Name
     * @param propertyValue Property Value
     * @return this object for chaining builder methods
     */
    BaseEvent& addEventProperty(const char *propertyName, const std::string &propertyValue);

    /**
     * Add a Raw Event Property Name and Value
     * @param propertyName Property Name
     * @param propertyValue Property Value
     * @return this object for chaining builder methods
     */
    BaseEvent& addEventProperty(const char *propertyName, double propertyValue);

 private:
    BaseEvent();

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

#endif  // BRANCHIO_EVENT_BASEEVENT_H_
