// Copyright (c) 2019 Branch Metrics, Inc.

#ifndef BRANCHIO_EVENT_STANDARDEVENT_H__
#define BRANCHIO_EVENT_STANDARDEVENT_H__

#include "BranchIO/Event/Event.h"

namespace BranchIO {

/**
 * Class for creating Standard Branch events for tracking and analytical purpose.
 */
class BRANCHIO_DLL_EXPORT StandardEvent : public Event {
 public:
    /**
     * Standard Event Types
     */
    typedef enum tagType {
        // Commerce events
        ADD_TO_CART = 0,                ///< Standard Add To Cart
        ADD_TO_WISHLIST,                ///< Standard Add To Wishlist
        VIEW_CART,                      ///< Standard View Cart
        INITIATE_PURCHASE,              ///< Standard Initiate Purchase
        ADD_PAYMENT_INFO,               ///< Standard Add Payment Information
        PURCHASE,                       ///< Standard Purchase Event
        SPEND_CREDITS,                  ///< Standard Spend Credits

        // Content events
        SEARCH,                         ///< Search Event
        VIEW_ITEM,                      ///< View Item Event
        VIEW_ITEMS,                     ///< View Items Event
        RATE,                           ///< Rate Event
        SHARE,                          ///< Share Event

        // User lifecycle events
        COMPLETE_REGISTRATION,          ///< Complete Registration
        COMPLETE_TUTORIAL,              ///< Complete Tutorial
        ACHIEVE_LEVEL,                  ///< Achieve Level
        UNLOCK_ACHIEVEMENT,             ///< Unlock Achievement

        // V2 Events
        INVITE,                         ///< Invite Event
        LOGIN,                          ///< Login Event
        RESERVE,                        ///< Reserve Event
        SUBSCRIBE,                      ///< Subscribe Event
        START_TRIAL,                    ///< Start Trial Event
        CLICK_AD,                       ///< Click Ad Event
        VIEW_AD                         ///< View Ad Event
    } Type;
    /**
     * Convert a StandardEvent to a String
     * @param eventType event type
     * @return a string representation of this class.
     */
    static const char *toString(StandardEvent::Type eventType);

    /**
     * Constructor.
     * This constructor can be used for creating Branch Standard Events.
     * @param eventType Standard Event Type.
     */
    explicit StandardEvent(StandardEvent::Type eventType);
};

}  // namespace BranchIO

#endif  // BRANCHIO_EVENT_STANDARDEVENT_H__
