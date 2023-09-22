// Copyright (c) 2019-21 Branch Metrics, Inc.

#ifndef BRANCHIO_EVENT_SESSIONEVENT_H__
#define BRANCHIO_EVENT_SESSIONEVENT_H__

#include <string>

#include "BranchIO/Event/Event.h"
#include "BranchIO/Util/Storage.h"

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
        std::string identity = Storage::instance().getString("session.identity");
        if (!identity.empty())
            addEventProperty(Defines::JSONKEY_APP_IDENTITY, identity);
    }

    /**
     * Set the Link URL
     * @param url Referring link, or an empty string if none.
     * @return this object for chaining builder methods
     */
    BaseEvent& setLinkUrl(const String& url);
};

}  // namespace BranchIO

#endif  // BRANCHIO_EVENT_SESSIONEVENT_H__
