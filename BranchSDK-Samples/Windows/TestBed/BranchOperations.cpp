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
std::wstring BranchOperations::s_branchKey;
std::wstring BranchOperations::s_uriScheme;

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

    outputTextField->appendText(wstring(L"Opening '") + url + L"'");

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
BranchOperations::initBranch(const std::wstring& branchKey, const std::wstring& uriScheme, const std::wstring& initialUrl, TextField* textField)
{
    InitializeCriticalSection(&lock);
    InitializeConditionVariable(&openCompleteCondition);

    outputTextField = textField;
    s_branchKey = branchKey;
    s_uriScheme = uriScheme;
    setupSDKLogging();

    BRANCH_LOG_I("TestBed launched with argument \"" << BranchIO::String(initialUrl).str() << "\"");

    // Now initialize the SDK
    AppInfo appInfo;
    appInfo.setAppVersion("1.0");

    branch = Branch::create(branchKey, &appInfo);

    /*
     * The Windows Advertising Identifier requires a UWP dependency and is not ordinarily available
     * for Win32. If you have it available, you can pass it here.
     */
    branch->getAdvertiserInfo().addId(AdvertiserInfo::WINDOWS_ADVERTISING_ID, "my-waid");

    wstring url(initialUrl);
    if (url[0] == '"') {
        // Strip off any leading and trailing quotes
        url = url.substr(1, url.length() - 2);
    }
    wstring::size_type prefixLength = min(uriScheme.length(), url.length());
    wstring prefix = url.substr(0, prefixLength);
    if (!url.empty() && prefix == uriScheme)
    {
        // Open any URI passed at the command line
        openURL(url);
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
    outputTextField->appendText(wstring(L"Initialized Branch SDK v") + branch->getVersionW() + L" with key " + s_branchKey);
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
        // It's necessary to send a new /v1/open after enabling tracking to obtain
        // a fresh session_id, or subsequent calls fail. This can be rolled into the
        // SDK. This is what the Web SDK does, e.g.
        openURL(L"");
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

std::wstring
BranchOperations::getSessionInfo()
{
    // Branch::getSessionInfo() returns SessionInfo&.
    // Branch::getSessionInfo() const returns const SessionInfo&.
    // The first is protected. The second is public. To access it, have to cast to
    // const Branch*.
    const Branch* cBranch(branch);
    auto const& sessionInfo = cBranch->getSessionInfo();

    wstring result(L"Session Info:\r\n");
    String deviceFingerprintId(sessionInfo.getStringProperty(Defines::JSONKEY_SESSION_FINGERPRINT));
    String identityId(sessionInfo.getStringProperty(Defines::JSONKEY_SESSION_IDENTITY));
    String sessionId(sessionInfo.getStringProperty(Defines::JSONKEY_SESSION_ID));

    /*
     * If tracking is disabled, identity_id and device_fingerprint_id are not sent to the API. They
     * are also removed from Registry storage. They remain in the SessionInfo object. Clearing them
     * in the SDK is troublesome without a potential API change, e.g. branch->disableTracking()
     * instead of branch->getAdvertiserInfo().disableTracking(). For now, just omit them from the
     * output here since they are not used in the SDK.
     */
    if (branch->getAdvertiserInfo().isTrackingDisabled()) {
        result += L" Device Fingerprint ID:\r\n";
        result += L" Identity ID:\r\n";
    }
    else {
        result += L" Device Fingerprint ID: " + deviceFingerprintId.wstr() + L"\r\n";
        result += L" Identity ID: " + identityId.wstr() + L"\r\n";
    }
    result += L" Session ID: " + sessionId.wstr() + L"\r\n";

    return result;
}