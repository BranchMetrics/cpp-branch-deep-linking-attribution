//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

#include <sstream>
#include <BranchIO/AppInfo.h>
#include <BranchIO/Branch.h>
#include <BranchIO/JSONObject.h>

using namespace UWP101;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Core;
using Windows::ApplicationModel::Core::CoreApplication;
using BranchIO::Branch;
using std::wostringstream;

MainPage::MainPage() :
	m_branch(nullptr),
	m_callback(this, &MainPage::onSuccess, &MainPage::onError, &MainPage::onStatus)
{
	InitializeComponent();
	BranchIO::AppInfo appInfo;
	appInfo
		.setAppVersion("1.0")
		.setCountryCode("US")
		.setDeveloperIdentity("Branch Metrics")
		.setEnvironment("FULL_APP")
		.setLanguage("en");

	m_branch = Branch::create("key_live_bcMneVUDd5sM5vD4BzfcbikdwrmnRDA8", &appInfo);

	auto branchDesc = m_branch->getVersion();
	std::wstring wBranchDesc(branchDesc.begin(), branchDesc.end());
	branchDescription->Text = ref new String(wBranchDesc.c_str());
}

MainPage::~MainPage()
{
	delete m_branch;
}

void
MainPage::onSuccess(int id, BranchIO::JSONObject response)
{
	auto jsonPayload(response.stringify(2));
	std::wstring wJson(jsonPayload.begin(), jsonPayload.end());
	displayResponse(L"Successful Response", wJson);
}

void
MainPage::onStatus(int id, int code, std::wstring message)
{
	displayResponse(L"Status Update", message);
}

void
MainPage::onError(int id, int code, std::wstring message)
{
	displayResponse(L"Request Failed", message);
}

void
MainPage::displayResponse(const std::wstring& header, const std::wstring& message)
{
	// Construct the message to display on this background thread
	MainPage^ page = this;
	wostringstream woss;
	woss << L"[" << header << L"]\n" << message << L"\n";
	auto displayMessage = ref new String(woss.str().c_str());

	// the update to the TextBox has to be performed on the main thread
	CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([page, displayMessage]()
			{
				page->responseText->Text += displayMessage;
			}));
}

void
MainPage::sendOpenRequest(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	std::string const url("https://hello-branch.app.link/0urbx9YcQR");
	std::wstring const wurl(url.begin(), url.end());
	wostringstream woss;
	woss << L"\n[Sending open request for " << wurl << L"]\n";
	responseText->Text += ref new String(woss.str().c_str());
	m_branch->openSession(url, &m_callback);
}

void
MainPage::responseTextUpdated(Windows::UI::Xaml::Controls::TextBox^ sender, Windows::UI::Xaml::Controls::TextBoxTextChangingEventArgs^ e)
{
	scrollToBottom(sender);
}

void
MainPage::scrollToBottom(Windows::UI::Xaml::Controls::TextBox^ textBox)
{
	// https://stackoverflow.com/a/40114621/875725
	Grid^ grid = static_cast<Grid^>(VisualTreeHelper::GetChild(textBox, 0));
	for (int i = 0; i < VisualTreeHelper::GetChildrenCount(grid); ++i)
	{
		Object^ child = VisualTreeHelper::GetChild(grid, i);
		ScrollViewer^ scrollViewer = dynamic_cast<ScrollViewer^>(child);
		if (scrollViewer == nullptr) continue;
		double const horizontalOffset = 0;
		double const verticalOffset = scrollViewer->ExtentHeight;
		float const zoomFactor = 1;

		scrollViewer->ChangeView(horizontalOffset, verticalOffset, zoomFactor);
		break;
	}
}