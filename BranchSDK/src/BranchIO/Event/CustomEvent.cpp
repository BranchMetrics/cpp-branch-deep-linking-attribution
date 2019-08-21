// Copyright (c) 2019 Branch Metrics, Inc.

#include "BranchIO/Event/CustomEvent.h"
#include "BranchIO/Defines.h"

namespace BranchIO {

CustomEvent::CustomEvent(const std::string &eventName) :
    Event(Defines::APIEndpoint::TRACK_CUSTOM_EVENT, eventName) { }

}  // namespace BranchIO
