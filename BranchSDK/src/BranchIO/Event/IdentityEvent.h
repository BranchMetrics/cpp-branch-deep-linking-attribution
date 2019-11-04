// Copyright (c) 2019 Branch Metrics, Inc.

#ifndef BRANCHIO_EVENT_IDENTITYEVENT_H__
#define BRANCHIO_EVENT_IDENTITYEVENT_H__

#include <string>

#include "BranchIO/Event/Event.h"
#include "BranchIO/JSONObject.h"

namespace BranchIO {

/**
 * (Internal) User Identity Login Event
 */
class BRANCHIO_DLL_EXPORT IdentityLoginEvent : public Event {
 public:
    /**
     * Constructor.
     * @param identity A value containing the unique identifier of the user
     */
    IdentityLoginEvent(std::string identity) :
            Event(Defines::APIEndpoint::IDENTIFY_USER, "setIdentity") {

        Event::addEventProperty(Defines::JSONKEY_APP_IDENTITY, identity);
    }
};

/**
 * (Internal) User Identity Logout Event
 */
class BRANCHIO_DLL_EXPORT IdentityLogoutEvent : public Event {
public:
    /**
     * Constructor.
     */
    explicit IdentityLogoutEvent() :
            Event(Defines::APIEndpoint::LOGOUT, "logout") {
    }
};

}  // namespace BranchIO

#endif  // BRANCHIO_EVENT_IDENTITYEVENT_H__
