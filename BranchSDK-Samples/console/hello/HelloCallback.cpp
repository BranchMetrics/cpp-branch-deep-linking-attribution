#include "HelloCallback.h"

#include <Poco/Clock.h>

#include <BranchIO/Util/Log.h>

HelloCallback::HelloCallback() : mResponseCount(0) {}

void HelloCallback::onSuccess(int id, BranchIO::JSONObject jsonResponse) {
    BRANCH_LOG_I("Success for ID " << id);
    BRANCH_LOG_V("Response: " << jsonResponse.stringify());
    incrementResponseCount();
}

void HelloCallback::onError(int id, int error, std::string description) {
    BRANCH_LOG_E("Error for ID " << id << ": (" << error << ") " << description);
    incrementResponseCount();
}

void HelloCallback::onStatus(int id, int error, std::string description) {
    BRANCH_LOG_W("Status for ID " << id << ": (" << error << ") " << description);
}

unsigned int HelloCallback::getResponseCount() const {
    Poco::Mutex::ScopedLock _l(mMutex);
    return mResponseCount;
}

void HelloCallback::incrementResponseCount() {
    Poco::Mutex::ScopedLock _l(mMutex);
    ++ mResponseCount;
    mIncrementCondition.broadcast();
}

void HelloCallback::waitForResponses(int expectedCount, long milliseconds) const {
    Poco::Clock start;
    long remaining(milliseconds);

    Poco::Mutex::ScopedLock _l(mMutex);
    while (mResponseCount < expectedCount && remaining > 0)
    {
        mIncrementCondition.tryWait(mMutex, remaining);
        remaining = milliseconds - static_cast<long>(start.elapsed() * 1.e-3);
    }
}

void HelloCallback::reset() {
    Poco::Mutex::ScopedLock _l(mMutex);
    mResponseCount = 0;
}
