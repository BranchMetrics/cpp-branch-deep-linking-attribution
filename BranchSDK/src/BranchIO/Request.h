// Copyright (c) 2019-21 Branch Metrics, Inc.

#ifndef BRANCHIO_REQUEST_H__
#define BRANCHIO_REQUEST_H__

#include <mutex>
#include <cstdint>

#include "BranchIO/fwd.h"
#include "BranchIO/Defines.h"
#include "BranchIO/Util/IClientSession.h"
#include "BranchIO/Util/Sleeper.h"

namespace BranchIO {

/**
 * (Internal) Class for making Server Requests.
 */
class Request {
 public:
    /// Maximum number of attempts before giving up
    static int const MaxAttemptCount;

    /// Maximum retry delay in ms
    static int32_t const MaxBackoffMillis;

    /**
     * Default constructor
     */
    Request();

    /**
     * Send this request to the Branch server (synchronous).
     * @param api API Endpoint
     * @param jsonPayload JSON Payload to send
     * @param callback Interface for success and failure response.
     * @param clientSession IClientSession to use for the request
     * @return The response body
     */
    JSONObject send(
        Defines::APIEndpoint api,
        const JSONObject& jsonPayload,
        IRequestCallback &callback,
        IClientSession *clientSession);

    /**
     * Get attempt count
     * @return the number of attempts so far (including any in progress)
     */
    int getAttemptCount() const;

    /**
     * Cancel this request.
     */
    void cancel();

    /**
     * Determine if this request was canceled.
     * @return true if canceled, false otherwise
     */
    bool isCanceled() const;

 protected:
    /**
     * Get the number of milliseconds to wait until the next request,
     * based on the attempt count. Returns 8000, 16000, 32000, 64000, then
     * 120000 for all successive attempts.
     *
     * @return a delay in ms
     * @todo(jdee): Determine a proper backoff scheme and maximum number of attempts.
     */
    int32_t getBackoffMillis() const;

    /**
     * Increment the attempt count returned by getAttemptCount().
     * @return the new attempt count
     */
    int incrementAttemptCount();

    /**
     * Sets the count returned by getAttemptCount() to 0.
     * @return *this
     */
    Request& resetAttemptCount();

 private:
    mutable std::mutex _mutex;
    int volatile _attemptCount;
    bool volatile _canceled;
    Sleeper _sleeper;
};

}  // namespace BranchIO

#endif  // BRANCHIO_REQUEST_H__
