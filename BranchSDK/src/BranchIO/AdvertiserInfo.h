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
     * @return this object for chainging buider methods
     */
    AdvertiserInfo &addId(AdIdType type, const std::string &value);

 private:
    const char *toXString(AdvertiserInfo::AdIdType idType) const;
};

}  // namespace BranchIO

#endif  // BRANCHIO_ADVERTISERINFO_H__
