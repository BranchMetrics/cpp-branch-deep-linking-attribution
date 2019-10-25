// Copyright (c) 2019 Branch Metrics, Inc.

#ifndef BRANCHIO_EVENT_IDENTITYEVENT_H__
#define BRANCHIO_EVENT_IDENTITYEVENT_H__

#include <string>

#include "BranchIO/Event/Event.h"
#include "BranchIO/JSONObject.h"

namespace BranchIO {

/**
 * (Internal) User Identity Event
 */
class BRANCHIO_DLL_EXPORT IdentityLoginEvent : public Event {
 public:
    /**
     * Constructor.
     * @param identity A value containing the unique identifier of the user
     */
    IdentityLoginEvent(std::string identity) :
            Event(Defines::APIEndpoint::IDENTIFY_USER, "setIdentity", NULL) {

        Event::addEventProperty(Defines::JSONKEY_APP_IDENTITY, identity);
    }
};


}  // namespace BranchIO

#endif  // BRANCHIO_EVENT_IDENTITYEVENT_H__
