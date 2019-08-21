// Copyright (c) 2019 Branch Metrics, Inc.

#ifndef BRANCHIO_ICLIENTSESSION_H__
#define BRANCHIO_ICLIENTSESSION_H__

#include <string>

#include "BranchIO/fwd.h"

namespace BranchIO {

/**
 * (Internal)
 * @todo(jdee): Document
 */
struct IClientSession {
    virtual ~IClientSession() {}

    /**
     * @todo(jdee): Document
     */
    virtual void stop() = 0;

    /**
     * Send this request as a "Post"
     * @param path API Endpoint
     * @param jsonPayload JSON Payload to send
     * @param callback Interface for success and failure response.
     * @return true if successful
     */
    virtual bool post(
        const std::string& path,
        const JSONObject& jsonPayload,
        IRequestCallback& callback) = 0;
};

}  // namespace BranchIO

#endif  // BRANCHIO_ICLIENTSESSION_H__
