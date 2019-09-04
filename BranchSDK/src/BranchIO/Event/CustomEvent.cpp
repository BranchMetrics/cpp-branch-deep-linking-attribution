// Copyright (c) 2019 Branch Metrics, Inc.

#include "BranchIO/Event/CustomEvent.h"
#include "BranchIO/Defines.h"

namespace BranchIO {

namespace Event {

CustomEvent::CustomEvent(const std::string &eventName) :
    Event(Defines::APIEndpoint::TRACK_CUSTOM_EVENT, eventName) { }

}  // namespace Event

}  // namespace BranchIO
