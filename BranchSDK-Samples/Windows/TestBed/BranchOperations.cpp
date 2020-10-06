#include "BranchOperations.h"

#include <BranchIO/Branch.h>
#include <BranchIO/Event/CustomEvent.h>
#include <BranchIO/Event/StandardEvent.h>
#include <BranchIO/LinkInfo.h>
#include <BranchIO/Util/Log.h>

#include "ScopeLock.h"
#include "TextField.h"

using namespace BranchIO;
using namespace std;

BranchIO::Branch* BranchOperations::branch(nullptr);
TextField* BranchOperations::outputTextField(nullptr);
bool volatile BranchOperations::openComplete(false);
CRITICAL_SECTION BranchOperations::lock;
CONDITION_VARIABLE BranchOperations::openCompleteCondition;
BranchIO::JSONObject BranchOperations::openResponse;

void
BranchOperations::setupSDKLogging()
{
    // Note: Debug and Verbose levels compiled out in Release builds
    Log::setLevel(Log::Verbose);
    const char* appDataPath = getenv("LocalAppData");
    string branchLogFilePath;
    if (appDataPath) {
        /*
         * By default, put log file in %LocalAppData%\Branch\TestBed, e.g. C:\Users\<username>\AppData\Local\Branch\TestBed
         */
        branchLogFilePath = appDataPath;
        branchLogFilePath += "\\Branch";
        // May fail if the directory already exists. (Ignore return value.)
        (void)_wmkdir(String(branchLogFilePath).wstr().c_str());

        branchLogFilePath += "\\TestBed";
        // May fail if the directory already exists. (Ignore return value.)
        (void)_wmkdir(String(branchLogFilePath).wstr().c_str());
    }
    else {
        // If the %LocalAppData% env. var. is not set for some reason, use the cwd.
        branchLogFilePath = String(_wgetcwd(nullptr, 0)).str();
    }

    // Generated and rolled over in this directory.
    ostringstream oss;
    oss << branchLogFilePath << "\\branch-sdk-" << GetCurrentProcessId() << ".log";
    Log::enableFileLogging(oss.str());
}

void
BranchOperations::openURL(const std::wstring& url)
{
    {
        ScopeLock l(lock);
        openComplete = false;
        openResponse = JSONObject();
    }

    outputTextField->appendText(wstring(L"Opening ") + url);

    struct OpenCallback : IRequestCallback
    {
        void onSuccess(int id, JSONObject payload)
        {
            outputTextField->appendText(wstring(L"Successful open response: ") + String(payload.stringify()).wstr());
            done(payload);
        }

        void onStatus(int id, int code, string message)
        {
            outputTextField->appendText(wstring(L"Branch open status: ") + String(message).wstr());
        }

        void onError(int id, int code, string message)
        {
            outputTextField->appendText(wstring(L"Branch open error: ") + String(message).wstr());
            done(JSONObject());
        }

    private:
        void done(const JSONObject& response)
        {
            {
                ScopeLock l(lock);
                openResponse = response;
                openComplete = true;
                WakeAllConditionVariable(&openCompleteCondition);
            }
            delete this;
        }
    };

    branch->openSession(url, new OpenCallback);
}

void
BranchOperations::initBranch(const std::wstring& initialUrl, TextField* textField)
{
    InitializeCriticalSection(&lock);
    InitializeConditionVariable(&openCompleteCondition);

    outputTextField = textField;
    setupSDKLogging();

    // Now initialize the SDK
    AppInfo appInfo;
    appInfo.setAppVersion("1.0");

    branch = Branch::create(BRANCH_KEY, &appInfo);

    wstring::size_type prefixLength = min(wstring(BRANCH_URI_SCHEME).length(), initialUrl.length());
    wstring prefix = initialUrl.substr(0, prefixLength);
    if (!initialUrl.empty() && prefix == BRANCH_URI_SCHEME)
    {
        // Open any URI passed at the command line
        openURL(initialUrl);
    }
    else {
        openURL(L"");
    }

    // Just delete when we exit
    atexit(shutDownBranch);
}

JSONObject
BranchOperations::getOpenResponse()
{
    ScopeLock l(lock);
    return openResponse;
}

void
BranchOperations::waitForOpen(DWORD dwMilliseconds)
{
    ScopeLock l(lock);
    while (!openComplete)
    {
        // Break if the sleep fails (real error) or the timer expires.
        if (!SleepConditionVariableCS(&openCompleteCondition, &lock, dwMilliseconds)) return;
    }
}

void
BranchOperations::shutDownBranch()
{
    if (branch) delete branch;
    branch = nullptr;

    DeleteCriticalSection(&lock);
}

void
BranchOperations::showInitializationMessage()
{
    outputTextField->appendText(wstring(L"Initialized Branch SDK v") + branch->getVersionW() + L" with key " + BRANCH_KEY);
    if (branch->getAdvertiserInfo().isTrackingDisabled())
    {
        outputTextField->appendText(L"Tracking disabled");
    }
    else
    {
        outputTextField->appendText(L"Tracking enabled");
    }
}

void
BranchOperations::login(const std::wstring& username)
{
    struct LoginCallback : IRequestCallback
    {
        void onSuccess(int id, JSONObject payload)
        {
            outputTextField->appendText(wstring(L"Successful login response: ") + String(payload.stringify()).wstr());
            done();
        }

        void onStatus(int id, int code, string message)
        {
            outputTextField->appendText(wstring(L"Branch login status: ") + String(message).wstr());
        }

        void onError(int id, int code, string message)
        {
            outputTextField->appendText(wstring(L"Branch login error: ") + String(message).wstr());
            done();
        }

    private:
        void done()
        {
            delete this;
        }
    };

    branch->setIdentity(username, new LoginCallback);
}

void
BranchOperations::logout()
{
    struct LogoutCallback : IRequestCallback
    {
        void onSuccess(int id, JSONObject payload)
        {
            outputTextField->appendText(wstring(L"Successful logout response: ") + String(payload.stringify()).wstr());
            done();
        }

        void onStatus(int id, int code, string message)
        {
            outputTextField->appendText(wstring(L"Branch logout status: ") + String(message).wstr());
        }

        void onError(int id, int code, string message)
        {
            outputTextField->appendText(wstring(L"Branch logout error: ") + String(message).wstr());
            done();
        }

    private:
        void done()
        {
            delete this;
        }
    };

    branch->logout(new LogoutCallback);
}

void
BranchOperations::logStandardEvent()
{
    StandardEvent event(StandardEvent::Type::PURCHASE);
    event
        .setCoupon("TestCoupon")
        .setCurrency("USD")
        .setDescription("Test Description")
        .setRevenue(99.99)
        .setSearchQuery("Some Search Query")
        .setShipping(1.99)
        .setTax(.99)
        .setTransactionId("Transaction123");

    string eventJson(event.toJSON().stringify());
    outputTextField->appendText(wstring(L"Sending standard event: ") + String(eventJson).wstr());

    struct StandardEventCallback : IRequestCallback
    {
        void onSuccess(int id, JSONObject payload)
        {
            outputTextField->appendText(wstring(L"Successful standard event response: ") + String(payload.stringify()).wstr());
            done();
        }

        void onStatus(int id, int code, string message)
        {
            outputTextField->appendText(wstring(L"Branch standard event status: ") + String(message).wstr());
        }

        void onError(int id, int code, string message)
        {
            outputTextField->appendText(wstring(L"Branch standard event error: ") + String(message).wstr());
            done();
        }

    private:
        void done()
        {
            delete this;
        }
    };

    branch->sendEvent(event, new StandardEventCallback);
}

void
BranchOperations::logCustomEvent()
{
    CustomEvent event(L"MyCustomEvent");
    event.setAdType(Event::AdType::BANNER);
    event.addCustomDataProperty(L"foo", L"Bar");

    string eventJson(event.toJSON().stringify());
    outputTextField->appendText(wstring(L"Sending custom event: ") + String(eventJson).wstr());

    struct CustomEventCallback : IRequestCallback
    {
        void onSuccess(int id, JSONObject payload)
        {
            outputTextField->appendText(wstring(L"Successful custom event response: ") + String(payload.stringify()).wstr());
            done();
        }

        void onStatus(int id, int code, string message)
        {
            outputTextField->appendText(wstring(L"Branch custom event status: ") + String(message).wstr());
        }

        void onError(int id, int code, string message)
        {
            outputTextField->appendText(wstring(L"Branch custom event error: ") + String(message).wstr());
            done();
        }

    private:
        void done()
        {
            delete this;
        }
    };

    branch->sendEvent(event, new CustomEventCallback);
}

void
BranchOperations::getShortURL()
{
    struct LinkRequest : LinkInfo, virtual IRequestCallback
    {
        LinkRequest(const std::wstring& canonicalUrl)
        {
            setFeature(L"testing");
            addControlParameter(L"$canonical_url", canonicalUrl);
            addControlParameter(L"$desktop_url", canonicalUrl);
            addControlParameter(L"$desktop_web_open_delay_ms", "3000");
        }

        void send(Branch* branch)
        {
            createUrl(branch, this);
        }

        void onSuccess(int id, JSONObject payload)
        {
            outputTextField->appendText(wstring(L"Successfully generated URL: ") + String(payload.stringify()).wstr());
            done();
        }

        void onStatus(int id, int code, string message)
        {
            outputTextField->appendText(wstring(L"Status getting URL: ") + String(message).wstr());
        }

        void onError(int id, int code, string message)
        {
            outputTextField->appendText(wstring(L"Error getting URL: ") + String(message).wstr());
            done();
        }

    private:
        void done()
        {
            delete this;
        }
    };

    // TODO: Find a better place for this page.
    LinkRequest* request = new LinkRequest(L"https://jdee.github.io/example-win32.html");
    outputTextField->appendText(wstring(L"Getting URL: ") + String(request->toString()).wstr());
    request->send(branch);
}

void
BranchOperations::closeSession()
{
    outputTextField->appendText(L"Closing session");

    struct CloseCallback : IRequestCallback
    {
        void onSuccess(int id, JSONObject payload)
        {
            outputTextField->appendText(wstring(L"Successful close response: ") + String(payload.stringify()).wstr());
            done();
        }

        void onStatus(int id, int code, string message)
        {
            outputTextField->appendText(wstring(L"Branch close status: ") + String(message).wstr());
        }

        void onError(int id, int code, string message)
        {
            outputTextField->appendText(wstring(L"Branch close error: ") + String(message).wstr());
            done();
        }
    private:
        void done()
        {
            delete this;
        }
    };

    branch->closeSession(new CloseCallback);
}

std::wstring
BranchOperations::getTrackingButtonLabel()
{
    const auto isDisabled = branch->getAdvertiserInfo().isTrackingDisabled();
    return isDisabled ? L"Enable Tracking" : L"Disable Tracking";
}

void
BranchOperations::toggleTracking()
{
    const auto isDisabled = branch->getAdvertiserInfo().isTrackingDisabled();
    if (isDisabled)
    {
        branch->getAdvertiserInfo().enableTracking();
        outputTextField->appendText(L"Tracking enabled");
    }
    else
    {
        branch->getAdvertiserInfo().disableTracking();
        outputTextField->appendText(L"Tracking disabled");
    }
}

std::wstring
BranchOperations::getIdentity()
{
    return branch->getIdentityW();
}