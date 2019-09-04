// Copyright (c) 2019 Branch Metrics, Inc.

#ifndef BRANCHIO_EVENT_SESSIONEVENT_H__
#define BRANCHIO_EVENT_SESSIONEVENT_H__

#include <string>

#include "BranchIO/Event/Event.h"
#include "BranchIO/JSONObject.h"

namespace BranchIO {

namespace Event {

/**
 * (Internal) Session Management Event
 */
class BRANCHIO_DLL_EXPORT SessionEvent : public Event {
 public:
    /**
     * Constructor.
     * @param apiEndpoint API endpoint
     * @param eventName Event Name
     * @param jsonPtr pre-filled JSONObject to seed this class.
     */
    SessionEvent(Defines::APIEndpoint apiEndpoint, const std::string &eventName, JSONObject::Ptr jsonPtr = NULL) :
        Event(apiEndpoint, eventName, jsonPtr) {}
};

/**
 * (Internal) Session Management Event -- Open
 */
class BRANCHIO_DLL_EXPORT SessionOpenEvent : public SessionEvent {
 public:
     /**
      * Constructor.
      * @param jsonPtr pre-filled JSONObject to seed this class.
      */
    explicit SessionOpenEvent(JSONObject::Ptr jsonPtr = NULL) :
        SessionEvent(Defines::APIEndpoint::REGISTER_OPEN, "Open", jsonPtr) {}

    /**
     * Set the Link URL
     * @param url Referring link, or an empty string if none.
     * @return this object for chaining builder methods
     */
    virtual Event& setLinkUrl(const std::string &url) {
        if (url.length() > 0) {
            addEventProperty(Defines::JSONKEY_APP_LINK_URL, url);
        }
        return *this;
    }
};

/**
 * (Internal) Session Management Event -- View
 */
class BRANCHIO_DLL_EXPORT SessionViewEvent: public SessionEvent {
 public:
     /**
      * Constructor.
      * @param jsonPtr pre-filled JSONObject to seed this class.
      */
    explicit SessionViewEvent(JSONObject::Ptr jsonPtr = NULL) :
        SessionEvent(Defines::APIEndpoint::REGISTER_VIEW, "View", jsonPtr) {}
};

/**
 * (Internal) Session Management Event -- Session Close
 */
class BRANCHIO_DLL_EXPORT SessionCloseEvent: public SessionEvent {
 public:
     /**
      * Constructor.
      * @param jsonPtr pre-filled JSONObject to seed this class.
      */
    explicit SessionCloseEvent(JSONObject::Ptr jsonPtr = NULL) :
        SessionEvent(Defines::APIEndpoint::REGISTER_CLOSE, "Close", jsonPtr) {}
};

}

}  // namespace BranchIO

#endif  // BRANCHIO_EVENT_SESSIONEVENT_H__
