#include <Poco/Condition.h>

#include <BranchIO/IRequestCallback.h>
#include <BranchIO/JSONObject.h>

/**
 * Callback for HelloWorld.
 * Typically a callback would just have onSuccess and onError, however as this
 * is a console app we want to wait for all events to complete before exiting.
 */
class HelloCallback : public virtual BranchIO::IRequestCallback
{
public:
    HelloCallback();

    virtual void onSuccess(int id, BranchIO::JSONObject jsonResponse);
    virtual void onError(int id, int error, std::string description);
    virtual void onStatus(int id, int error, std::string description);

    unsigned int getResponseCount() const;
    void waitForResponses(int expectedCount, long milliseconds) const;

 private:
    void incrementResponseCount();
    void reset();

    mutable Poco::Mutex mMutex;
    mutable Poco::Condition mIncrementCondition;
    unsigned int volatile mResponseCount;
};
