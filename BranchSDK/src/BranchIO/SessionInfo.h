// Copyright (c) 2019 Branch Metrics, Inc.

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
     * Set the Session Fingerprint ID.
     * @param deviceFingerprint as issued by the Branch server
     * @param isPersistent also stored in persistence if true
     * @return This object for chaining builder methods
     */
    virtual SessionInfo& setFingerprintId(const std::string &deviceFingerprint, bool isPersistent = true);

    /**
     * Set the Session Identity Id.
     * @param identity as issued by the Branch server
     * @return This object for chaining builder methods
     */
    virtual SessionInfo& setIdentityId(const std::string &identity);

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

}  // namespace BranchIO

#endif  // BRANCHIO_SESSIONINFO_H__
