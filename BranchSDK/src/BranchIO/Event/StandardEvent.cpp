// Copyright (c) 2019 Branch Metrics, Inc.

#include "BranchIO/Event/StandardEvent.h"
#include "BranchIO/Defines.h"

namespace BranchIO {

// Branch Standard Events
const char *SE_ADD_TO_CART = "ADD_TO_CART";
const char *SE_ADD_TO_WISHLIST = "ADD_TO_WISHLIST";
const char *SE_VIEW_CART = "VIEW_CART";
const char *SE_INITIATE_PURCHASE = "INITIATE_PURCHASE";
const char *SE_ADD_PAYMENT_INFO = "ADD_PAYMENT_INFO";
const char *SE_PURCHASE = "PURCHASE";
const char *SE_SPEND_CREDITS = "SPEND_CREDITS";
const char *SE_SEARCH = "SEARCH";
const char *SE_VIEW_ITEM = "VIEW_ITEM";
const char *SE_VIEW_ITEMS = "VIEW_ITEMS";
const char *SE_RATE = "RATE";
const char *SE_SHARE = "SHARE";
const char *SE_COMPLETE_REGISTRATION = "COMPLETE_REGISTRATION";
const char *SE_COMPLETE_TUTORIAL = "COMPLETE_TUTORIAL";
const char *SE_ACHIEVE_LEVEL = "ACHIEVE_LEVEL";
const char *SE_UNLOCK_ACHIEVEMENT = "UNLOCK_ACHIEVEMENT";
const char *SE_INVITE = "INVITE";
const char *SE_LOGIN = "LOGIN";
const char *SE_RESERVE = "RESERVE";
const char *SE_SUBSCRIBE = "SUBSCRIBE";
const char *SE_START_TRIAL = "START_TRIAL";
const char *SE_CLICK_AD = "CLICK_AD";
const char *SE_VIEW_AD = "VIEW_AD";

StandardEvent::StandardEvent(StandardEvent::Type eventType) :
    Event(Defines::APIEndpoint::TRACK_STANDARD_EVENT, StandardEvent::toString(eventType)) { }

const char *
StandardEvent::toString(StandardEvent::Type eventType) {
    switch (eventType) {
        case StandardEvent::ADD_TO_CART:
            return SE_ADD_TO_CART;
        case StandardEvent::ADD_TO_WISHLIST:
            return SE_ADD_TO_WISHLIST;
        case StandardEvent::VIEW_CART:
            return SE_VIEW_CART;
        case StandardEvent::INITIATE_PURCHASE:
            return SE_INITIATE_PURCHASE;
        case StandardEvent::ADD_PAYMENT_INFO:
            return SE_ADD_PAYMENT_INFO;
        case StandardEvent::PURCHASE:
            return SE_PURCHASE;
        case StandardEvent::SPEND_CREDITS:
            return SE_SPEND_CREDITS;
        case StandardEvent::SEARCH:
            return SE_SEARCH;
        case StandardEvent::VIEW_ITEM:
            return SE_VIEW_ITEM;
        case StandardEvent::VIEW_ITEMS:
            return SE_VIEW_ITEMS;
        case StandardEvent::RATE:
            return SE_RATE;
        case StandardEvent::SHARE:
            return SE_SHARE;
        case StandardEvent::COMPLETE_REGISTRATION:
            return SE_COMPLETE_REGISTRATION;
        case StandardEvent::COMPLETE_TUTORIAL:
            return SE_COMPLETE_TUTORIAL;
        case StandardEvent::ACHIEVE_LEVEL:
            return SE_ACHIEVE_LEVEL;
        case StandardEvent::UNLOCK_ACHIEVEMENT:
            return SE_UNLOCK_ACHIEVEMENT;
        case StandardEvent::INVITE:
            return SE_INVITE;
        case StandardEvent::LOGIN:
            return SE_LOGIN;
        case StandardEvent::RESERVE:
            return SE_RESERVE;
        case StandardEvent::SUBSCRIBE:
            return SE_SUBSCRIBE;
        case StandardEvent::START_TRIAL:
            return SE_START_TRIAL;
        case StandardEvent::CLICK_AD:
            return SE_CLICK_AD;
        case StandardEvent::VIEW_AD:
            return SE_VIEW_AD;
    }

    return Defines::NO_BRANCH_VALUE;
}

}  // namespace BranchIO
