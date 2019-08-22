// Copyright (c) 2019 Branch Metrics, Inc.

#ifndef BRANCHIO_BRANCH_H__
#define BRANCHIO_BRANCH_H__

#include <Poco/Mutex.h>

#include <string>

#include "BranchIO/dll.h"
#include "BranchIO/AdvertiserInfo.h"
#include "BranchIO/AppInfo.h"
#include "BranchIO/DeviceInfo.h"
#include "BranchIO/Event/Event.h"
#include "BranchIO/PackagingInfo.h"
#include "BranchIO/IRequestCallback.h"
#include "BranchIO/SessionInfo.h"

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
    static Branch *create(
        const std::string &branchKey,
        AppInfo *pInfo);

    /**
     * Destructor.
     */
    virtual ~Branch();

    /**
     * Initialize/Open a Branch Session.
     * @param linkUrl Referring link, or an empty string if none.
     * @param callback Callback to fire with success or failure notification.
     */
    virtual void openSession(const std::string &linkUrl = "", IRequestCallback *callback = NULL);

    /**
     * Close a Branch Session.
     * @param callback Callback to fire with success or failure notification.
     */
    virtual void closeSession(IRequestCallback *callback = NULL);

    /**
     * Send an event to Branch.
     * @param event Event to send
     * @param callback Callback to fire with success or failure notification.
     */
    virtual void sendEvent(const Event &event, IRequestCallback *callback);

    /**
     * @return the SDK Version
     */
    static const std::string &getVersion();

 public:
    /**
     * @todo(andyp): Revisit Scope.
     * @return the Branch Key used to initialize Branch.
     */
    std::string getBranchKey() const;

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
    mutable Poco::Mutex _mutex;
    RequestManager * volatile _requestManager;
    PackagingInfo _packagingInfo;
};

}  // namespace BranchIO

#endif  // BRANCHIO_BRANCH_H__
