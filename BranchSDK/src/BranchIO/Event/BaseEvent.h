// Copyright (c) 2019-21 Branch Metrics, Inc.

#ifndef BRANCHIO_EVENT_BASEEVENT_H_
#define BRANCHIO_EVENT_BASEEVENT_H_

#include <functional>
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
    virtual std::string name() const;

    /**
     * Adds a custom data property associated with this Branch Event.
     * Note that these properties are protocol-specific, and may not be sent in all cases.
     * @param propertyName Name of the custom property
     * @param propertyValue Value of the custom property
     * @return This object for chaining builder methods
     */
    virtual BaseEvent& addCustomDataProperty(const String &propertyName, const String &propertyValue);

    /**
     * Get the custom data properties associated with this Branch Event.
     * @return the Custom Data for the event.
     */
    virtual JSONObject getCustomData() const;

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

    /**
     * Set an optional function to be invoked on successful completion of the event. Defaults
     * to a no-op.
     * @param resultHandler a function accepting a reference to a const JSONObject, which will receive the result
     * @return *this
     */
    BaseEvent& setResultHandler(const std::function<void(const JSONObject&)>& resultHandler) {
        Poco::Mutex::ScopedLock _l(mMutex);
        mResultHandler = resultHandler;
        return *this;
    }

    /**
     * Invoke the result handler for this event
     * @param result the result to pass to the handler
     */
    void handleResult(const JSONObject& result) const {
        Poco::Mutex::ScopedLock _l(mMutex);
        mResultHandler(result);
    }

 protected:
    /**
     * Constructor
     * @param apiEndpoint Endpoint this event will be directed towards
     * @param eventName The "Name" of the event
     * @param jsonPtr Base JSON to start with
     */
    BaseEvent(Defines::APIEndpoint apiEndpoint, const String& eventName, JSONObject::Ptr jsonPtr = nullptr);

    /**
     * Add a Raw Event Property Name and Value
     * @param propertyName Property Name
     * @param propertyValue Property Value
     * @return this object for chaining builder methods
     */
    BaseEvent& addEventProperty(const char *propertyName, const String& propertyValue);

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
    Poco::Mutex mutable mMutex;

    // API Endpoint
    Defines::APIEndpoint mAPIEndpoint;

    // The Event Name
    std::string mEventName;

    // Custom Data is rendered as a sibling to the event data
    JSONObject mCustomData;

    // Result callback
    std::function<void(const JSONObject&)> mResultHandler;
};

}  // namespace BranchIO

#endif  // BRANCHIO_EVENT_BASEEVENT_H_
