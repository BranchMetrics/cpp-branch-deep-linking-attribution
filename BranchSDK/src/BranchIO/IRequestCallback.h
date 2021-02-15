// Copyright (c) 2019-21 Branch Metrics, Inc.

#ifndef BRANCHIO_IREQUESTCALLBACK_H__
#define BRANCHIO_IREQUESTCALLBACK_H__

#include <string>
#include "BranchIO/JSONObject.h"

namespace BranchIO {

/**
 * Interface to be called following a successful (or unsuccessful) server request.
 */
class BRANCHIO_DLL_EXPORT IRequestCallback {
 public:
    IRequestCallback() {}
    virtual ~IRequestCallback() {}

    /**
     * Callback when a successful request has been made.
     * @param id Request Id (future)
     * @param jsonResponse JSON Response data
     * @todo(andyp): Revisit id Context
     */
    virtual void onSuccess(int id, JSONObject jsonResponse) = 0;

    /**
     * Callback when an error has been detected. This indicates a final failure
     * of the request. This method or onSuccess will be called once for each
     * event.
     * @param id Request Id (future)
     * @param error an Integer Error Code
     * @param description a human-readable description of the error for debugging.
     * @todo(andyp): Revisit Context
     */
    virtual void onError(int id, int error, std::string description) = 0;

    /**
     * Callback when a transient error has been detected. Multiple calls to
     * onStatus may be received before a final onSuccess or onError.
     * @param id Request Id (future)
     * @param error an Integer Error Code
     * @param description a human-readable description of the error for debugging.
     * @todo(andyp): Revisit Context
     */
    virtual void onStatus(int id, int error, std::string description) = 0;
};

}  // namespace BranchIO

#endif  // BRANCHIO_IREQUESTCALLBACK_H__
