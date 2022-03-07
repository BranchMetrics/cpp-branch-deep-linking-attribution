// Copyright (c) 2019-21 Branch Metrics, Inc.

#include "BranchIO/Request.h"

#include <algorithm>
#include <string>
#include <mutex>

#include "BranchIO/IRequestCallback.h"
#include "BranchIO/Util/Log.h"
#include "BranchIO/Util/StringUtils.h"

using namespace std;

namespace BranchIO {

int const Request::MaxAttemptCount = 5;
int32_t const Request::MaxBackoffMillis = 120000;

Request::Request() : _attemptCount(0), _canceled(false) { }

JSONObject Request::send(
    Defines::APIEndpoint api,
    const JSONObject& jsonPayload,
    IRequestCallback &callback,
    IClientSession *clientSession) {

    string uri(Defines::stringify(api));
    std::string path(Defines::stringify(api));
    if (path.empty()) {
        path = "/";
    }

    JSONObject result;
    while (!isCanceled() && getAttemptCount() < MaxAttemptCount) {
        // POST the request
        if (clientSession->post(path, jsonPayload, callback, result)) {
            break;
        }

        // POST failed
        incrementAttemptCount();

        if (getAttemptCount() >= MaxAttemptCount) {
            break;
        }

        int32_t backoff = getBackoffMillis();
        BRANCH_LOG_D("POST failed. Retrying in " << backoff << " ms");

        _sleeper.sleep(backoff);
    }

    if (getAttemptCount() >= MaxAttemptCount) {
        BRANCH_LOG_E("Maximum number of retries reached.");
        callback.onError(0, 0, "Maximum number of retries reached.");
        return result;
    }

    if (isCanceled()) {
        BRANCH_LOG_W("Request canceled");
        // Don't call the user's error callback after cancellation.
        // @todo(jdee): Review this.
        callback.onStatus(0, 0, "Request canceled");
        return result;
    }

    BRANCH_LOG_V("POST Success");
    return result;
}

void
Request::cancel() {
    scoped_lock _l(_mutex);
    _canceled = true;
    _sleeper.wake();
}

bool
Request::isCanceled() const {
    scoped_lock _l(_mutex);
    return _canceled;
}

int32_t
Request::getBackoffMillis() const {
    int32_t backoff(8000);
    int attemptCount(getAttemptCount());

    while (--attemptCount > 0 && backoff < MaxBackoffMillis) backoff *= 2;

    return min(MaxBackoffMillis, backoff);
}

int
Request::getAttemptCount() const {
    scoped_lock _l(_mutex);
    return _attemptCount;
}

int
Request::incrementAttemptCount() {
    scoped_lock _l(_mutex);
    return ++_attemptCount;
}

Request&
Request::resetAttemptCount() {
    scoped_lock _l(_mutex);
    _attemptCount = 0;
    return *this;
}

}  // namespace BranchIO
