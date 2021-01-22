// Copyright (c) 2019-21 Branch Metrics, Inc.

#include "BranchIO/Event/CustomEvent.h"
#include "BranchIO/Defines.h"

namespace BranchIO {

CustomEvent::CustomEvent(const String& eventName) :
    Event(Defines::APIEndpoint::TRACK_CUSTOM_EVENT, eventName) { }

}  // namespace BranchIO
