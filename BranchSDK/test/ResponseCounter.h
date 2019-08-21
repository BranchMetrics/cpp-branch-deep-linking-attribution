#ifndef __RESPONSE_COUNTER_H__
#define __RESPONSE_COUNTER_H__

#include <Poco/Clock.h>
#include <Poco/Condition.h>
#include <Poco/Mutex.h>

#include <BranchIO/IRequestCallback.h>

/**
 * Synchronized mock callback. Tracks the number of responses received.
 */
struct ResponseCounter : public virtual BranchIO::IRequestCallback
{
    ResponseCounter() : mResponseCount(0) {}

    // ----- IRequestCallback -----

    void onSuccess(int id, BranchIO::JSONObject jsonResponse)
    {
        incrementResponseCount();
    }

    void onError(int id, int error, std::string description)
    {
        incrementResponseCount();
    }

    void onStatus(int id, int error, std::string description)
    {
        
    }

    // ----- Synchronized response monitoring -----

    unsigned int getResponseCount() const
    {
        Poco::Mutex::ScopedLock _l(mMutex);
        return mResponseCount;
    }

    void incrementResponseCount()
    {
        Poco::Mutex::ScopedLock _l(mMutex);
        ++ mResponseCount;
        mIncrementCondition.broadcast();
    }

    /**
     * Wait until the specified number of responses is received or the
     * specified time interval has expired.
     */
    void waitForResponses(int expectedCount, long milliseconds) const
    {
        Poco::Clock start;
        long remaining(milliseconds);

        Poco::Mutex::ScopedLock _l(mMutex);
        while (mResponseCount < expectedCount && remaining > 0)
        {
            mIncrementCondition.tryWait(mMutex, remaining);
            remaining = milliseconds - static_cast<long>(start.elapsed() * 1.e-3);
        }
    }

    mutable Poco::Mutex mMutex;
    mutable Poco::Condition mIncrementCondition;
    unsigned int volatile mResponseCount;
};

#endif // __RESPONSE_COUNTER_H__
