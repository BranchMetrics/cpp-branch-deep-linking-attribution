// Copyright (c) 2019 Branch Metrics, Inc.

#include "BranchIO/Request.h"

#include <Poco/URI.h>
#include <algorithm>
#include <string>

#include "BranchIO/Util/IClientSession.h"
#include "BranchIO/IRequestCallback.h"
#include "BranchIO/Util/Log.h"

using namespace std;
using namespace Poco;

namespace BranchIO {

int const Request::MaxAttemptCount = 5;
int32_t const Request::MaxBackoffMillis = 120000;

Request::Request() : _attemptCount(0), _canceled(false) { }

void Request::send(
    Defines::APIEndpoint api,
    const JSONObject& jsonPayload,
    IRequestCallback &callback,
    IClientSession *clientSession) {

    URI uri(Defines::toString(api));
    std::string path(uri.getPathAndQuery());
    if (path.empty()) {
        path = "/";
    }

    while (!isCanceled() && getAttemptCount() < MaxAttemptCount &&
           !clientSession->post(path, jsonPayload, callback)) {
        // POST failed
        incrementAttemptCount();

        int32_t backoff = getBackoffMillis();
        BRANCH_LOG_W("POST failed. Retrying in " << backoff << " ms");

        _sleeper.sleep(backoff);
    }

    if (getAttemptCount() >= MaxAttemptCount) {
        BRANCH_LOG_E("Maximum number of failures reached.");
        callback.onError(0, 0, "Maximum number of failures reached.");
        return;
    }

    if (isCanceled()) {
        BRANCH_LOG_W("Request canceled");
        // Don't call the user's callback after cancellation.
        // @todo(jdee): Review this.
        return;
    }

    BRANCH_LOG_I("POST Success");
}

void
Request::cancel() {
    Mutex::ScopedLock _l(_mutex);
    _canceled = true;
    _sleeper.wake();
}

bool
Request::isCanceled() const {
    Mutex::ScopedLock _l(_mutex);
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
    Mutex::ScopedLock _l(_mutex);
    return _attemptCount;
}

int
Request::incrementAttemptCount() {
    Mutex::ScopedLock _l(_mutex);
    return ++_attemptCount;
}

Request&
Request::resetAttemptCount() {
    Mutex::ScopedLock _l(_mutex);
    _attemptCount = 0;
    return *this;
}

}  // namespace BranchIO
