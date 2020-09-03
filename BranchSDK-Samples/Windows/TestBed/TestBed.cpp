// TestBed.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "TestBed.h"

#include "Button.h"
#include "TextField.h"

#include <memory>

#include <BranchIO/Branch.h>
#include <BranchIO/Util/Log.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

using namespace BranchIO;
using namespace std;

Branch* branch(nullptr);
static constexpr wchar_t const* const BRANCH_KEY = L"key_live_oiT8IkxqCmpGcDT35ttO1fkdExktZD1x";
static constexpr wchar_t const* const BRANCH_URI_SCHEME = L"testbed";

// Static layout
// TODO: Define further buttons here
static int const ID_TEXT_FIELD = 1000;
static int const ID_OPEN_BUTTON = 1001;
static int const ID_LOGIN_BUTTON = 1002;
static int const ID_LOGOUT_BUTTON = 1003;

TextField outputTextField(L"Initializing...", 440, 20, 400, 400, ID_TEXT_FIELD);
Button openButton(L"Open", 20, 20, 400, 50, ID_OPEN_BUTTON);
Button loginButton(L"Login", 20, 90, 400, 50, ID_LOGIN_BUTTON);
Button logoutButton(L"Logout", 20, 160, 400, 50, ID_LOGOUT_BUTTON);

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

static
Branch*
initBranch(const std::wstring& key)
{
    AppInfo appInfo;
    appInfo.setAppVersion("1.0");

    return Branch::create(key, &appInfo);
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
            outputTextField.appendText(wstring(L"Branch status: ") + String(message).wstr());
        }

        void onError(int id, int code, string message)
        {
            outputTextField.appendText(wstring(L"Branch error: ") + String(message).wstr());
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
            outputTextField.appendText(wstring(L"Branch status: ") + String(message).wstr());
        }

        void onError(int id, int code, string message)
        {
            outputTextField.appendText(wstring(L"Branch error: ") + String(message).wstr());
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
            outputTextField.appendText(wstring(L"Branch status: ") + String(message).wstr());
        }

        void onError(int id, int code, string message)
        {
            outputTextField.appendText(wstring(L"Branch error: ") + String(message).wstr());
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

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    Log::setLevel(Log::Verbose);
    // Generated and rolled over in the current directory (x64\Debug).
    Log::enableFileLogging("branch-sdk.log");
    Log::enableSystemLogging();

    // Initialize Branch
    branch = initBranch(BRANCH_KEY);
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

   openButton.setButtonPressCallback([]() {
       openURL(L"https://win32.app.link/crtafBueu9");
   });
   loginButton.setButtonPressCallback([]() {
       login(L"user1");
   });
   logoutButton.setButtonPressCallback([]() {
       logout();
   });

   /*
    * TODO: Set up further buttons here.
    */

   outputTextField.appendText(wstring(L"Initialized with key ") + BRANCH_KEY);

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

            // TODO: Add further buttons here
            case ID_OPEN_BUTTON:
                openButton.onPress();
                break;
            case ID_LOGIN_BUTTON:
                loginButton.onPress();
                break;
            case ID_LOGOUT_BUTTON:
                logoutButton.onPress();
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
