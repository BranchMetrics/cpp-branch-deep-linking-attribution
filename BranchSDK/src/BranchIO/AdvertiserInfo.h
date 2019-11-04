// Copyright (c) 2019 Branch Metrics, Inc.

#ifndef BRANCHIO_ADVERTISERINFO_H__
#define BRANCHIO_ADVERTISERINFO_H__

#include <string>
#include "BranchIO/PropertyManager.h"

namespace BranchIO {

/**
 * Advertiser ID Information.
 */
class BRANCHIO_DLL_EXPORT AdvertiserInfo : public PropertyManager {
 public:
    /**
     * Advertiser ID Types.
     */
     typedef enum tagAdIdType {
         IDFA,
         GOOGLE_ADVERTISING_ID,
         WINDOWS_ADVERTISING_ID,
         ROKU_RIDA,
         SAMSUNG_IFA,
         LG_IFA,
         PANASONIC_IFA,
         PLAYSTATION_IFA,
         XBOX_MSAI
     } AdIdType;

    /**
     * Constructor.
     */
    AdvertiserInfo();
    virtual ~AdvertiserInfo();

    /**
     * Add an advertiser/hardware ID
     * @param type Advertiser Id Type
     * @param value Advertiser Id Value
     * @return this object for chaining builder methods
     */
    AdvertiserInfo &addId(AdIdType type, const std::string &value);

    /**
     * Method to disable the Tracking state. If disabled SDK will not track any user data or state.
     * The SDK will not send any network calls except for deep linking when tracking is disabled.
     * Tracking is "on" by default.
     * @return this object for chaining builder methods
     */
    AdvertiserInfo &disableTracking();

    /**
     * Method to enable the Tracking state. If disabled SDK will not track any user data or state.
     * The SDK will not send any network calls except for deep linking when tracking is disabled.
     * Tracking is "on" by default.
     * @return this object for chaining builder methods
     */
    AdvertiserInfo &enableTracking();

    /**
     * "Limit Ad Tracking" (LAT) enables users to opt-out and therefore restrict advertisers from targeting based on user behavior.
     * @param is_lat true if the User has chosen to limit ad tracking
     * @return
     */
    AdvertiserInfo &limitAdTracking(bool is_lat);

 public:
    /**
     * Determine if tracking has been marked as disabled.
     * @return true if Tracking has been marked as disabled.
     */
    bool isTrackingDisabled() const;

    /**
     * Determine if tracking has been marked as limited.
     * @return true if Tracking has been marked as limited.
     */
    bool isTrackingLimited() const;

 private:
    bool trackingDisabled;
    bool trackingLimited;

    static constexpr const char *const AdvertiserStorage = "advertiser";
    static const char *stringify(AdvertiserInfo::AdIdType idType);
};

}  // namespace BranchIO

#endif  // BRANCHIO_ADVERTISERINFO_H__
