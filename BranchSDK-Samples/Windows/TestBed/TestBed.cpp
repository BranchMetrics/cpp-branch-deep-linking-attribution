// TestBed.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "TestBed.h"

#include "Button.h"
#include "TextField.h"

#include <algorithm>
#include <memory>

#include <BranchIO/Branch.h>
#include <BranchIO/Event/CustomEvent.h>
#include <BranchIO/Event/StandardEvent.h>
#include <BranchIO/LinkInfo.h>
#include <BranchIO/Util/Log.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

using namespace BranchIO;
using namespace std;

Branch* volatile branch(nullptr);
static constexpr wchar_t const* const BRANCH_KEY = L"key_live_oiT8IkxqCmpGcDT35ttO1fkdExktZD1x";
static constexpr wchar_t const* const BRANCH_URI_SCHEME = L"testbed:";

// Static layout
// Define any further buttons here
static int const ID_TEXT_FIELD = 1000;
static int const ID_OPEN_BUTTON = 1001;
static int const ID_LOGIN_BUTTON = 1002;
static int const ID_LOGOUT_BUTTON = 1003;
static int const ID_STANDARD_EVENT_BUTTON = 1004;
static int const ID_CUSTOM_EVENT_BUTTON = 1005;
static int const ID_GET_SHORT_URL_BUTTON = 1006;
static int const ID_CLOSE_BUTTON = 1007;

TextField outputTextField(L"Initializing...", 440, 20, 400, 400, ID_TEXT_FIELD);
Button openButton(L"Open", 20, 20, 190, 50, ID_OPEN_BUTTON);
Button closeButton(L"Close", 20, 90, 190, 50, ID_CLOSE_BUTTON);
Button loginButton(L"Login", 20, 160, 190, 50, ID_LOGIN_BUTTON);
Button logoutButton(L"Logout", 20, 230, 190, 50, ID_LOGOUT_BUTTON);
Button standardEventButton(L"Standard Event", 230, 20, 190, 50, ID_STANDARD_EVENT_BUTTON);
Button customEventButton(L"Custom Event", 230, 90, 190, 50, ID_CUSTOM_EVENT_BUTTON);
Button getShortURLButton(L"Get Short URL", 230, 160, 190, 50, ID_GET_SHORT_URL_BUTTON);

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

static
void
setupBranchSDKLogging()
{
    // Note: Debug and Verbose levels compiled out in Release builds
    Log::setLevel(Log::Verbose);
    const char* appDataPath = getenv("AppData");
    string branchLogFilePath;
    if (appDataPath) {
        /*
         * By default, put log file in %AppData%\Branch\TestBed, e.g. C:\Users\<username>\AppData\Roaming\Branch\TestBed
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
        // If the %AppData% env. var. is not set for some reason, use the cwd.
        branchLogFilePath = String(_wgetcwd(nullptr, 0)).str();
    }

    // Generated and rolled over in this directory.
    Log::enableFileLogging(branchLogFilePath + "\\branch-sdk.log");
}

static
void
openURL(const std::wstring& url)
{
    outputTextField.appendText(wstring(L"Opening ") + url);

    struct OpenCallback : IRequestCallback
    {
        void onSuccess(int id, JSONObject payload)
        {
            outputTextField.appendText(wstring(L"Successful open response: ") + String(payload.stringify()).wstr());
            done();
        }

        void onStatus(int id, int code, string message)
        {
            outputTextField.appendText(wstring(L"Branch open status: ") + String(message).wstr());
        }

        void onError(int id, int code, string message)
        {
            outputTextField.appendText(wstring(L"Branch open error: ") + String(message).wstr());
            done();
        }

    private:
        void done()
        {
            delete this;
        }
    };

    branch->openSession(url, new OpenCallback);
}

static
void
initBranch(const std::wstring& key, const std::wstring& initialUrl)
{
    setupBranchSDKLogging();

    // Now initialize the SDK
    AppInfo appInfo;
    appInfo.setAppVersion("1.0");
    // TODO: Sort this out.
    appInfo.setDeveloperIdentity("abc");

    branch = Branch::create(key, &appInfo);

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
}

static
void
login(const std::wstring& username)
{
    struct LoginCallback : IRequestCallback
    {
        void onSuccess(int id, JSONObject payload)
        {
            outputTextField.appendText(wstring(L"Successful login response: ") + String(payload.stringify()).wstr());
            done();
        }

        void onStatus(int id, int code, string message)
        {
            outputTextField.appendText(wstring(L"Branch login status: ") + String(message).wstr());
        }

        void onError(int id, int code, string message)
        {
            outputTextField.appendText(wstring(L"Branch login error: ") + String(message).wstr());
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

static
void
logout()
{
    struct LogoutCallback : IRequestCallback
    {
        void onSuccess(int id, JSONObject payload)
        {
            outputTextField.appendText(wstring(L"Successful logout response: ") + String(payload.stringify()).wstr());
            done();
        }

        void onStatus(int id, int code, string message)
        {
            outputTextField.appendText(wstring(L"Branch logout status: ") + String(message).wstr());
        }

        void onError(int id, int code, string message)
        {
            outputTextField.appendText(wstring(L"Branch logout error: ") + String(message).wstr());
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

static
void
logStandardEvent()
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
    outputTextField.appendText(wstring(L"Sending standard event: ") + String(eventJson).wstr());

    struct StandardEventCallback : IRequestCallback
    {
        void onSuccess(int id, JSONObject payload)
        {
            outputTextField.appendText(wstring(L"Successful standard event response: ") + String(payload.stringify()).wstr());
            done();
        }

        void onStatus(int id, int code, string message)
        {
            outputTextField.appendText(wstring(L"Branch standard event status: ") + String(message).wstr());
        }

        void onError(int id, int code, string message)
        {
            outputTextField.appendText(wstring(L"Branch standard event error: ") + String(message).wstr());
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

static
void
logCustomEvent()
{
    CustomEvent event(L"MyCustomEvent");
    event.setAdType(Event::AdType::BANNER);
    event.addCustomDataProperty(L"foo", L"Bar");

    string eventJson(event.toJSON().stringify());
    outputTextField.appendText(wstring(L"Sending custom event: ") + String(eventJson).wstr());

    struct CustomEventCallback : IRequestCallback
    {
        void onSuccess(int id, JSONObject payload)
        {
            outputTextField.appendText(wstring(L"Successful custom event response: ") + String(payload.stringify()).wstr());
            done();
        }

        void onStatus(int id, int code, string message)
        {
            outputTextField.appendText(wstring(L"Branch custom event status: ") + String(message).wstr());
        }

        void onError(int id, int code, string message)
        {
            outputTextField.appendText(wstring(L"Branch custom event error: ") + String(message).wstr());
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

static
void
getShortURL()
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
            outputTextField.appendText(wstring(L"Successfully generated URL: ") + String(payload.stringify()).wstr());
            done();
        }

        void onStatus(int id, int code, string message)
        {
            outputTextField.appendText(wstring(L"Status getting URL: ") + String(message).wstr());
        }

        void onError(int id, int code, string message)
        {
            outputTextField.appendText(wstring(L"Error getting URL: ") + String(message).wstr());
            done();
        }

    private:
        void done()
        {
            delete this;
        }
    };

    LinkRequest* request = new LinkRequest(L"https://branch.io");
    outputTextField.appendText(wstring(L"Getting URL: ") + String(request->toString()).wstr());
    request->send(branch);
}

static
void
closeSession()
{
    outputTextField.appendText(L"Closing session");

    struct CloseCallback : IRequestCallback
    {
        void onSuccess(int id, JSONObject payload)
        {
            outputTextField.appendText(wstring(L"Successful close response: ") + String(payload.stringify()).wstr());
            done();
        }

        void onStatus(int id, int code, string message)
        {
            outputTextField.appendText(wstring(L"Branch close status: ") + String(message).wstr());
        }

        void onError(int id, int code, string message)
        {
            outputTextField.appendText(wstring(L"Branch close error: ") + String(message).wstr());
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

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize Branch
    initBranch(BRANCH_KEY, lpCmdLine ? lpCmdLine : L"");
    // Delete the Branch instance after the message loop terminates
    unique_ptr<Branch> branchDeleter(branch);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TESTBED, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TESTBED));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TESTBED));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TESTBED);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);

   outputTextField.create(hWnd);
   openButton.create(hWnd);
   loginButton.create(hWnd);
   logoutButton.create(hWnd);
   standardEventButton.create(hWnd);
   customEventButton.create(hWnd);
   getShortURLButton.create(hWnd);
   closeButton.create(hWnd);

   openButton.setButtonPressCallback([]() {
       openURL(L"https://win32.app.link/crtafBueu9");
   });
   loginButton.setButtonPressCallback([]() {
       login(L"user1");
   });
   logoutButton.setButtonPressCallback([]() {
       logout();
   });
   standardEventButton.setButtonPressCallback([]() {
       logStandardEvent();
   });
   customEventButton.setButtonPressCallback([]() {
       logCustomEvent();
   });
   getShortURLButton.setButtonPressCallback([]() {
       getShortURL();
   });
   closeButton.setButtonPressCallback([]() {
       closeSession();
   });

   /*
    * Set up any further buttons here.
    */

   outputTextField.appendText(wstring(L"Initialized Branch SDK v") + branch->getVersionW() + L" with key " + BRANCH_KEY);

   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;

            // Add any further buttons here
            case ID_OPEN_BUTTON:
                openButton.onPress();
                break;
            case ID_LOGIN_BUTTON:
                loginButton.onPress();
                break;
            case ID_LOGOUT_BUTTON:
                logoutButton.onPress();
                break;
            case ID_STANDARD_EVENT_BUTTON:
                standardEventButton.onPress();
                break;
            case ID_CUSTOM_EVENT_BUTTON:
                customEventButton.onPress();
                break;
            case ID_GET_SHORT_URL_BUTTON:
                getShortURLButton.onPress();
                break;
            case ID_CLOSE_BUTTON:
                closeButton.onPress();
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
