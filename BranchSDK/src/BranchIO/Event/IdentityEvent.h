// Copyright (c) 2019 Branch Metrics, Inc.

#ifndef BRANCHIO_EVENT_IDENTITYEVENT_H__
#define BRANCHIO_EVENT_IDENTITYEVENT_H__

#include <string>

#include "BranchIO/Event/BaseEvent.h"

namespace BranchIO {

/**
 * (Internal) Identity Event
 */
class BRANCHIO_DLL_EXPORT IdentityEvent : public BaseEvent {
 public:
    /**
     * Constructor.
     * @param apiEndpoint API endpoint
     * @param eventName Event Name
     */
    IdentityEvent(Defines::APIEndpoint apiEndpoint, const String& eventName) :
        BaseEvent(apiEndpoint, eventName) {}
};

/**
 * (Internal) User Identity Login Event
 */
class BRANCHIO_DLL_EXPORT IdentityLoginEvent : public IdentityEvent {
 public:
    /**
     * Constructor.
     * @param identity A value containing the unique identifier of the user
     */
    IdentityLoginEvent(const String& identity) :
            IdentityEvent(Defines::APIEndpoint::IDENTIFY_USER, "setIdentity") {

        addEventProperty(Defines::JSONKEY_APP_IDENTITY, identity);
    }
};

/**
 * (Internal) User Identity Logout Event
 */
class BRANCHIO_DLL_EXPORT IdentityLogoutEvent : public IdentityEvent {
 public:
    /**
     * Constructor.
     */
    explicit IdentityLogoutEvent() :
            IdentityEvent(Defines::APIEndpoint::LOGOUT, "logout") {
    }
};

}  // namespace BranchIO

#endif  // BRANCHIO_EVENT_IDENTITYEVENT_H__
