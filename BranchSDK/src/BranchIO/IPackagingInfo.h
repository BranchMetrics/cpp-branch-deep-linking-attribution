// Copyright (c) 2019-21 Branch Metrics, Inc.

#ifndef BRANCHIO_IPACKAGINGINFO_H__
#define BRANCHIO_IPACKAGINGINFO_H__

#include <string>

#include "BranchIO/fwd.h"

namespace BranchIO {

/**
 * Interface representing information needed for packaging. Implemented by
 * Branch class.
 */
struct IPackagingInfo {
    virtual ~IPackagingInfo() {}

    /**
     * Get the Branch key in use
     * @return the Branch key
     */
    virtual std::string getBranchKey() const = 0;

    /**
     * Get a reference to a writable SessionInfo.
     * @return the SessionInfo
     */
    virtual SessionInfo& getSessionInfo() = 0;

    /**
     * Get a reference to a readonly SessionInfo.
     * @return the SessionInfo
     */
    virtual SessionInfo const& getSessionInfo() const = 0;

    /**
     * Get a reference to a readonly DeviceInfo.
     * @return the DeviceInfo
     */
    virtual DeviceInfo const& getDeviceInfo() const = 0;

    /**
     * Get a reference to a readonly AppInfo.
     * @return the AppInfo
     */
    virtual AppInfo const& getAppInfo() const = 0;

    /**
     * Get a reference to a readonly AdvertiserInfo.
     * @return the AdvertiserInfo
     */
    virtual AdvertiserInfo& getAdvertiserInfo() = 0;

    /**
   * Sets the setRequestMetaData.
   * @param requestMetaDataJsonString json string contaning key-pairs of metadata
   * @return *this
   */
    virtual void setRequestMetaData(JSONObject requestMetaData) = 0;

    /**
   * Gets a reference to requestMetaData.
   * @return *JSONObject containing metadata Key pair values
   */
    virtual JSONObject& getRequestMetaData() = 0;
};

}  // namespace BranchIO

#endif  // BRANCHIO_IPACKAGINGINFO_H__
