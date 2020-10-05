// Copyright (c) 2019 Branch Metrics, Inc.

#ifndef BRANCHIO_EVENT_SESSIONEVENT_H__
#define BRANCHIO_EVENT_SESSIONEVENT_H__

#include <string>

#include "BranchIO/Event/Event.h"
#include "BranchIO/Util/Storage.h"
#include "BranchIO/JSONObject.h"

namespace BranchIO {

/**
 * (Internal) Session Management Event
 */
class BRANCHIO_DLL_EXPORT SessionEvent : public BaseEvent {
 public:
    /**
     * Constructor.
     * @param apiEndpoint API endpoint
     * @param eventName Event Name
     */
    SessionEvent(Defines::APIEndpoint apiEndpoint, const String& eventName) :
        BaseEvent(apiEndpoint, eventName) {}
};

/**
 * (Internal) Session Management Event -- Open
 */
class BRANCHIO_DLL_EXPORT SessionOpenEvent : public SessionEvent {
 public:
    /**
     * Constructor.
     */
    SessionOpenEvent() :
        SessionEvent(Defines::APIEndpoint::REGISTER_OPEN, "Open") {

        IStorage& storage(Storage::instance());
        if (storage.has("session.identity_id")) {
            addEventProperty(Defines::JSONKEY_SESSION_IDENTITY, storage.getString("session.identity_id"));
        }
    }

    /**
     * Set the Link URL
     * @param url Referring link, or an empty string if none.
     * @return this object for chaining builder methods
     */
    virtual BaseEvent& setLinkUrl(const String &url) {
        if (url.str().length() > 0) {
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
      */
    explicit SessionViewEvent() :
        SessionEvent(Defines::APIEndpoint::REGISTER_VIEW, "View") {}
};

}  // namespace BranchIO

#endif  // BRANCHIO_EVENT_SESSIONEVENT_H__
