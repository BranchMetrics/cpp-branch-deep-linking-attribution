// Copyright (c) 2019-21 Branch Metrics, Inc.

#ifndef BRANCHIO_SESSIONINFO_H__
#define BRANCHIO_SESSIONINFO_H__

#include <string>
#include "BranchIO/PropertyManager.h"

namespace BranchIO {

/**
 * (Internal) Session State Information.
 */
class BRANCHIO_DLL_EXPORT SessionInfo : public PropertyManager {
 public:
    /**
     * Default constructor. Loads values from Storage if available.
     */
    SessionInfo();

    virtual ~SessionInfo();

    /**
     * Set the Session Randomized Device Token.
     * @param device token as issued by the Branch server
     * @return This object for chaining builder methods
     */
    virtual SessionInfo& setDeviceToken(const std::string &randomizedDeviceToken);

    /**
     * Set the Randomized Bundle Token.
     * @param bundleToken as issued by the Branch server
     * @return This object for chaining builder methods
     */
    virtual SessionInfo& setBundleToken(const std::string &bundleToken);

    /**
     * Set the Session Id.
     * @param sessionId as issued by the Branch server
     * @return This object for chaining builder methods
     */
    virtual SessionInfo& setSessionId(const std::string &sessionId);

    /**
     * @return true if there is a current session.
     */
    virtual bool hasSessionId() const;

 private:
    virtual SessionInfo& doAddProperty(const char *name, const std::string &value);
};

constexpr const char* const SESSIONSTORAGE = "session";

}  // namespace BranchIO

#endif  // BRANCHIO_SESSIONINFO_H__
