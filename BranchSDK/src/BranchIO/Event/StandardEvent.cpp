// Copyright (c) 2019-21 Branch Metrics, Inc.

#include "BranchIO/Event/StandardEvent.h"
#include "BranchIO/Defines.h"
#include "BranchIO/String.h"

namespace BranchIO {

// Branch Standard Events
static constexpr const char* const SE_ADD_TO_CART = "ADD_TO_CART";
static constexpr const char* const SE_ADD_TO_WISHLIST = "ADD_TO_WISHLIST";
static constexpr const char* const SE_VIEW_CART = "VIEW_CART";
static constexpr const char* const SE_INITIATE_PURCHASE = "INITIATE_PURCHASE";
static constexpr const char* const SE_ADD_PAYMENT_INFO = "ADD_PAYMENT_INFO";
static constexpr const char* const SE_PURCHASE = "PURCHASE";
static constexpr const char* const SE_SPEND_CREDITS = "SPEND_CREDITS";
static constexpr const char* const SE_SEARCH = "SEARCH";
static constexpr const char* const SE_VIEW_ITEM = "VIEW_ITEM";
static constexpr const char* const SE_VIEW_ITEMS = "VIEW_ITEMS";
static constexpr const char* const SE_RATE = "RATE";
static constexpr const char* const SE_SHARE = "SHARE";
static constexpr const char* const SE_COMPLETE_REGISTRATION = "COMPLETE_REGISTRATION";
static constexpr const char* const SE_COMPLETE_TUTORIAL = "COMPLETE_TUTORIAL";
static constexpr const char* const SE_ACHIEVE_LEVEL = "ACHIEVE_LEVEL";
static constexpr const char* const SE_UNLOCK_ACHIEVEMENT = "UNLOCK_ACHIEVEMENT";
static constexpr const char* const SE_INVITE = "INVITE";
static constexpr const char* const SE_LOGIN = "LOGIN";
static constexpr const char* const SE_RESERVE = "RESERVE";
static constexpr const char* const SE_SUBSCRIBE = "SUBSCRIBE";
static constexpr const char* const SE_START_TRIAL = "START_TRIAL";
static constexpr const char* const SE_CLICK_AD = "CLICK_AD";
static constexpr const char* const SE_VIEW_AD = "VIEW_AD";
static constexpr const char* const SE_INITIATE_STREAM = "INITIATE_STREAM";
static constexpr const char* const SE_COMPLETE_STREAM = "COMPLETE_STREAM";

StandardEvent::StandardEvent(StandardEvent::Type eventType) :
    Event(Defines::APIEndpoint::TRACK_STANDARD_EVENT, StandardEvent::stringify(eventType)) { }

const char *
StandardEvent::stringify(StandardEvent::Type eventType) {
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
        case StandardEvent::INITIATE_STREAM:
            return SE_INITIATE_STREAM;
        case StandardEvent::COMPLETE_STREAM:
            return SE_COMPLETE_STREAM;
    }

    return Defines::NO_BRANCH_VALUE;
}

}  // namespace BranchIO
