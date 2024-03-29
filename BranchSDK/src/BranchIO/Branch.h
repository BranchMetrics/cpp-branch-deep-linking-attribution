// Copyright (c) 2019-21 Branch Metrics, Inc.

#ifndef BRANCHIO_BRANCH_H__
#define BRANCHIO_BRANCH_H__

#include <mutex>
#include <string>

#include "BranchIO/dll.h"
#include "BranchIO/AdvertiserInfo.h"
#include "BranchIO/AppInfo.h"
#include "BranchIO/DeviceInfo.h"
#include "BranchIO/Event/BaseEvent.h"
#include "BranchIO/PackagingInfo.h"
#include "BranchIO/IRequestCallback.h"
#include "BranchIO/SessionInfo.h"
#include "BranchIO/String.h"

namespace BranchIO {

class RequestManager;

/**
 * The core object required when using Branch SDK. You should declare an object of this type at
 * the application-level.
 * <p>
 * Normal instantiation of this object would look like this:
 * </p>
 * <!--
 * <pre style="background:#fff;padding:10px;border:2px solid silver;">
 * Branch::create(myBranchKey, myDeviceInfo);  // from an application
 * </pre>
 * -->
 */
class BRANCHIO_DLL_EXPORT Branch {
 public:
    /**
     * Factory returns a new instance of the Branch SDK.
     * Caller is responsible for deletion.
     * @param branchKey Branch Application Key.
     * @param pInfo Application Information.
     * @return a new instance of Branch
     */
    static Branch *create(const String& branchKey, AppInfo* pInfo);

    /**
     * Destructor.
     */
    ~Branch();

    /**
     * Initialize/Open a Branch Session.
     * @param linkUrl Referring link, or an empty string if none.
     * @param callback Callback to fire with success or failure notification.
     */
    void openSession(const String& linkUrl = "", IRequestCallback* callback = nullptr);

    /**
     * Close a Branch Session.
     * **Note:** This function is a no-op. There is no need to call it.
     * @param callback Callback to fire with success or failure notification.
     */
    void closeSession(IRequestCallback *callback = nullptr);

    /**
     * Send an event to Branch.
     * @param event BaseEvent to send
     * @param callback Callback to fire with success or failure notification.
     */
    void sendEvent(const BaseEvent &event, IRequestCallback *callback);

    /*
     * @todo(jdee): Get rid of runtime getters for compile-time constants
     */

    /**
     * @return the SDK Version
     */
    static std::string getVersion();

    /**
     * @todo(andyp): Revisit Scope.
     * @return the Branch Key used to initialize Branch.
     */
    std::string getBranchKey() const;

    /**
     * @return the SDK Version as a UTF-16 string
     */
    static std::wstring getVersionW();

    /**
     * @return the Branch key as a UTF-16 string
     */
    std::wstring getBranchKeyW() const;

    /**
     * From IPackagingInfo
     * @return App information
     */
    const AppInfo &getAppInfo() const;

    /**
     * From IPackagingInfo
     * @return DeviceInfo device information
     */
    const DeviceInfo &getDeviceInfo() const;

    /**
     * From IPackagingInfo
     * @return SessionInfo session information
     */
    const SessionInfo &getSessionInfo() const;

    /**
     * From IPackagingInfo
     * @return Advertiser information
     */
    AdvertiserInfo &getAdvertiserInfo();

 public:
    // User Identity APIs.

    /**
     * Identifies the current user to the Branch API by supplying a unique identifier
     * @param userId   A value containing the unique identifier of the user.
     * @param callback Callback to fire with success or failure notification.
     */
    void setIdentity(const String& userId, IRequestCallback* callback);

    /**
     * This method should be called if you know that a different person is about to use the app. For example,
     * if you allow users to log out and let their friend use the app, you should call this to notify Branch
     * to create a new user for this device. This will clear the first and latest params, as a new session is created.</p>
     *
     * @param callback Callback to fire with success or failure notification.
     */
    void logout(IRequestCallback *callback);

    /**
     * Get the current developer identity as a UTF-8 string
     * @return the current developer identity (blank if none)
     */
    static std::string getIdentity();

    /**
     * Get the current developer identity as a UTF-16 string
     * @return the current developer identity (blank if none)
     */
    static std::wstring getIdentityW();

    /**
     * Sets  key-value pairs of metadata which is added in requests. These key-pair values 
     * are sent with every requests including installs and opens
     * @param key - name of the metadata key
     * @param value - value of the metadata
     */
    void setRequestMetaData(std::string key, std::string value);

    /**
     * Clears request metadata set by setRequestMetaData()
     */
    void clearRequestMetaData();

 private:
    /**
     * Explicitly shut down this Branch instance. Happens automatically
     * in destructor.
     */
    void stop();

    /**
     * Blocks until this Branch instance has finished shutting down.
     * Destructor also blocks.
     */
    void waitTillFinished();

    /**
    * Returns a JSON object with SessionID, Randomized bundle Token and link.
    * Its passed in callback in setIdentity and logout (to fake response from /v1/profile and v1/logout) 
    */
    //JSONObject & getIdentityCallbackReturnParams();
    void getIdentityCallbackReturnParams(JSONObject& identityParams);
 protected:
    Branch();

    /**
     * Synchronized getter for RequestManager.
     * @return a pointer to the RequestManager
     */
    RequestManager* getRequestManager() const;

    /**
     * (Internal) From IPackagingInfo
     * @return SessionInfo session information
     */
    SessionInfo &getSessionInfo();

 private:
    mutable std::mutex _mutex;
    RequestManager * volatile _requestManager;
    PackagingInfo _packagingInfo;
    static JSONObject requestMetaDataJsonObj;
};

}  // namespace BranchIO

#endif  // BRANCHIO_BRANCH_H__
