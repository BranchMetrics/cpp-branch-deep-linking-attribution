// Copyright (c) 2019-21 Branch Metrics, Inc.

#ifndef BRANCHIO_EVENT_CUSTOMEVENT_H__
#define BRANCHIO_EVENT_CUSTOMEVENT_H__

#include <string>

#include "BranchIO/Event/Event.h"

namespace BranchIO {

/**
 * Class for creating custom Branch events for tracking and analytical purpose.
 */
class BRANCHIO_DLL_EXPORT CustomEvent : public Event {
 public:
    /**
     * Constructor.
     * This constructor can be used for free-form Events or Branch Standard Events.
     * Event names that match Standard Events will be treated as Standard Events.
     * @param eventName Custom Event Name.
     */
    explicit CustomEvent(const String &eventName);
};

}  // namespace BranchIO

#endif  // BRANCHIO_EVENT_CUSTOMEVENT_H__
