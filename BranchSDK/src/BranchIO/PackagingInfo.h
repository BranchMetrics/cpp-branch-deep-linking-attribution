// Copyright (c) 2019-21 Branch Metrics, Inc.

#ifndef BRANCHIO_PACKAGINGINFO_H_
#define BRANCHIO_PACKAGINGINFO_H_

#include <string>
#include <mutex>

#include "BranchIO/AdvertiserInfo.h"
#include "BranchIO/AppInfo.h"
#include "BranchIO/DeviceInfo.h"
#include "BranchIO/IPackagingInfo.h"
#include "BranchIO/SessionInfo.h"


namespace BranchIO {

/**
 * Class to hold all info for packaging
 */
class PackagingInfo : public virtual IPackagingInfo {
 public:
    /**
     * Constructor
     * @param branchKey (optional) a Branch key to use at initialization
     */
    explicit PackagingInfo(const std::string& branchKey = std::string()) :
        _branchKey(branchKey) {}

    /**
     * Get the Branch key in use
     * @return the Branch key
     */
    std::string getBranchKey() const;

    /**
     * Set the Branch key
     * @param branchKey the Branch key
     * @return *this
     */
    PackagingInfo& setBranchKey(const std::string& branchKey);

    /**
     * Get a reference to a writable SessionInfo.
     * @return the SessionInfo
     */
    SessionInfo& getSessionInfo();

    /**
     * Get a reference to a readonly SessionInfo.
     * @return the SessionInfo
     */
    SessionInfo const& getSessionInfo() const;

    /**
     * Set the SessionInfo
     * @param sessionInfo the SessionInfo
     * @return *this
     */
    PackagingInfo& setSessionInfo(const SessionInfo& sessionInfo);

    /**
     * Get a reference to a readonly DeviceInfo.
     * @return the DeviceInfo
     */
    DeviceInfo const& getDeviceInfo() const;


    /**
     * Get a reference to a writable DeviceInfo.
     * @return the DeviceInfo
     */
    DeviceInfo& getDeviceInfo();

    /**
     * Set the DeviceInfo
     * @param deviceInfo the DeviceInfo
     * @return *this
     */
    PackagingInfo& setDeviceInfo(const DeviceInfo& deviceInfo);

    /**
     * Get a reference to a readonly AppInfo.
     * @return the AppInfo
     */
    AppInfo const& getAppInfo() const;


    /**
     * Get a reference to a writable AppInfo.
     * @return the AppInfo
     */
    AppInfo& getAppInfo();

    /**
     * Set the AppInfo
     * @param appInfo the AppInfo
     * @return *this
     */
    PackagingInfo& setAppInfo(const AppInfo& appInfo);

    /**
     * Get a reference to a readonly AdvertiserInfo.
     * @return the AdvertiserInfo
     */
    AdvertiserInfo& getAdvertiserInfo();

    /**
     * Set the AdvertiserInfo.
     * @param advertiserInfo the AdvertiserInfo
     * @return *this
     */
    PackagingInfo& setAdvertiserInfo(const AdvertiserInfo& advertiserInfo);

    /**
    * Sets the setRequestMetaData.
    * @param requestMetaDataJsonString json string contaning key-pairs of metadata
    * @return *this
    */
    void setRequestMetaData(JSONObject requestMetaData);

    /**
   * Gets a reference to requestMetaData.
   * @return *JSONObject containing metadata Key pair values
   */
    JSONObject& getRequestMetaData();

 private:
    mutable std::mutex _mutex;
#pragma warning(push)
#pragma warning(disable: 4251)
    std::string _branchKey;
#pragma warning(pop)
    AdvertiserInfo _advertiserInfo;
    AppInfo _appInfo;
    DeviceInfo _deviceInfo;
    SessionInfo _sessionInfo;
    JSONObject _requestMetaData;
};

}  // namespace BranchIO

#endif  // BRANCHIO_PACKAGINGINFO_H_
