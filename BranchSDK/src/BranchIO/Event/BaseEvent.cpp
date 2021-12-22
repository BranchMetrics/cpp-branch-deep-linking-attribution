// Copyright (c) 2019-21 Branch Metrics, Inc.

#include "BaseEvent.h"
#include "BranchIO/AppInfo.h"
#include "BranchIO/Defines.h"
#include "BranchIO/DeviceInfo.h"
#include "BranchIO/IPackagingInfo.h"
#include "BranchIO/JSONObject.h"
#include "BranchIO/SessionInfo.h"
#include "BranchIO/AdvertiserInfo.h"
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Foundation.Collections.h>

using Poco::Mutex;
using namespace std;

using namespace winrt::Windows::Data::Json;
using namespace winrt::Windows::Foundation::Collections;

namespace BranchIO {

// Top Level JSON Blocks
static const char *JSONKEY_NAME = "name";
static const char *JSONKEY_CUSTOM_DATA = "custom_data";
static const char *JSONKEY_EVENT_DATA = "event_data";
static const char *JSONKEY_USER_DATA = "user_data";
static const char *JSONKEY_BRANCH_KEY = "branch_key";
static const char *JSONKEY_ADVERTISING_IDS = "advertising_ids";

BaseEvent::BaseEvent(Defines::APIEndpoint apiEndpoint, const String& eventName, JSONObject::Ptr jsonPtr) :
    mAPIEndpoint(apiEndpoint),
    mEventName(eventName.str()),
    mResultHandler([](const JSONObject&) {}) {

    if (jsonPtr.get()) {
        // Copy the key/values
        IIterator<IKeyValuePair<winrt::hstring, IJsonValue>> it;
        JsonObject sourceJObject = jsonPtr->getWinRTJsonObj();
        for (it = sourceJObject.begin(); it != sourceJObject.end(); ++it) {
            IKeyValuePair< winrt::hstring, IJsonValue>  kvp = it.Current();
            jObject.SetNamedValue(kvp.Key(), kvp.Value());
        }
    }
}

BaseEvent::BaseEvent(const BaseEvent &other) :
    PropertyManager(other),
    mAPIEndpoint(other.mAPIEndpoint),
    mEventName(other.mEventName),
    mCustomData(other.mCustomData),
    mResultHandler(other.mResultHandler) {
}

BaseEvent::~BaseEvent() = default;

BaseEvent&
BaseEvent::addEventProperty(const char *propertyName, const String& propertyValue) {
    addProperty(propertyName, propertyValue);
    return *this;
}

BaseEvent&
BaseEvent::addEventProperty(const char *propertyName, double propertyValue) {
    addProperty(propertyName, propertyValue);
    return *this;
}

BaseEvent&
BaseEvent::addCustomDataProperty(const String &propertyName, const String &propertyValue) {
    Mutex::ScopedLock _l(mMutex);
    mCustomData.set(propertyName.str(), propertyValue.str());
    return *this;
}

JSONObject
BaseEvent::getCustomData() const {
    Mutex::ScopedLock _l(mMutex);
    return mCustomData;
}

std::string
BaseEvent::name() const {
    Mutex::ScopedLock _l(mMutex);
    return mEventName;
}

Defines::APIEndpoint
BaseEvent::getAPIEndpoint() const {
    Mutex::ScopedLock _l(mMutex);
    return mAPIEndpoint;
}

void
BaseEvent::packageRawEvent(JSONObject &jsonObject) const {
    jsonObject.parse(PropertyManager::stringify());
}

void
BaseEvent::packageV1Event(IPackagingInfo &packagingInfo, JSONObject &jsonObject) const {
    jsonObject += *this;
    jsonObject += packagingInfo.getSessionInfo().toJSON();
    jsonObject += packagingInfo.getDeviceInfo().toJSON();
    jsonObject += packagingInfo.getAppInfo().toJSON();

    string waid = packagingInfo.getAdvertiserInfo().getStringProperty(Defines::JSONKEY_WINDOWS_ADVERTISING_ID);
    if (!waid.empty()) {
        jsonObject.set(JSONKEY_ADVERTISING_IDS, packagingInfo.getAdvertiserInfo().toJSON());
    }

    if (getAPIEndpoint() == Defines::APIEndpoint::REGISTER_OPEN && jsonObject.has(Defines::JSONKEY_SESSION_ID)) {
        jsonObject.remove(Defines::JSONKEY_SESSION_ID);
    }

    // Ad Tracking Limited
    jsonObject.set(Defines::JSONKEY_APP_LAT_V1, (packagingInfo.getAdvertiserInfo().isTrackingLimited() ? 1 : 0));
}

void
BaseEvent::packageV2Event(IPackagingInfo &packagingInfo, JSONObject &jsonObject) const {
    // Set the event name
    jsonObject.set(JSONKEY_NAME, name());

    // Set the event data
    jsonObject.set(JSONKEY_EVENT_DATA, *this);

    // Set Custom Data (if any)
    if (!getCustomData().isEmpty()) {
        jsonObject.set(JSONKEY_CUSTOM_DATA, getCustomData());
    }

    // Set the user data
    JSONObject userData;
    JSONObject sessionInfo = packagingInfo.getSessionInfo().toJSON();
    JSONObject deviceInfo = packagingInfo.getDeviceInfo().toJSON();
    JSONObject appInfo = packagingInfo.getAppInfo().toJSON();
    JSONObject adInfo = packagingInfo.getAdvertiserInfo().toJSON();

    userData += sessionInfo;
    userData += deviceInfo;
    userData += appInfo;

    // Advertising Ids
    bool isAdTrackingLimited = packagingInfo.getAdvertiserInfo().isTrackingLimited();
    if (!isAdTrackingLimited && adInfo.size() > 0) {
        userData.set(JSONKEY_ADVERTISING_IDS, adInfo);
    }
    userData.set(Defines::JSONKEY_APP_LAT_V2, (isAdTrackingLimited ? 1 : 0));

    jsonObject.set(JSONKEY_USER_DATA, userData);
}

void
BaseEvent::package(IPackagingInfo &packagingInfo, JSONObject &jsonPackage) const {
    // Set the Branch Key
    jsonPackage.set(JSONKEY_BRANCH_KEY, packagingInfo.getBranchKey());

    switch (Defines::endpointType(getAPIEndpoint())) {
        case Defines::V1:
            packageV1Event(packagingInfo, jsonPackage);
            break;

        case Defines::V2:
            packageV2Event(packagingInfo, jsonPackage);
            break;

        case Defines::RAW:
        default:
            // Dump the raw event data into the request and hope for the best
            packageRawEvent(jsonPackage);
            break;
    }
}


}  // namespace BranchIO
