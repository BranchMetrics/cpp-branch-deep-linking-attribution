#ifndef __RESPONSE_COUNTER_H__
#define __RESPONSE_COUNTER_H__


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
        std::scoped_lock _l(mMutex);
        return mResponseCount;
    }

    void incrementResponseCount()
    {
        std::scoped_lock _l(mMutex);
        ++ mResponseCount;
        mIncrementCondition.notify_all();
    }

    /**
     * Wait until the specified number of responses is received or the
     * specified time interval has expired.
     */
    void waitForResponses(int expectedCount, long milliseconds) const
    {
        std::chrono::steady_clock::time_point t_start = std::chrono::steady_clock::now();
        long remaining(milliseconds);

        std::chrono::duration<int, std::milli> remainingDuration(remaining);

        std::unique_lock _l(mMutex);
        while (mResponseCount < expectedCount && remaining > 0)
        {
            mIncrementCondition.wait_for(_l, remainingDuration);
            auto t_now = std::chrono::steady_clock::now();
            double elapsed_time_ms = std::chrono::duration<double, std::milli>(t_now - t_start).count();
            remaining = milliseconds - static_cast<int32_t>(elapsed_time_ms);
        }
    }

    mutable std::mutex mMutex;
    mutable std::condition_variable mIncrementCondition;
    unsigned int volatile mResponseCount;
};

#endif // __RESPONSE_COUNTER_H__
