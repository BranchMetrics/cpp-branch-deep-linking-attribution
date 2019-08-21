//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"

#include <exception>
#include <BranchIO/Branch.h>
#include <BranchIO/IRequestCallback.h>

namespace BranchIO
{
	/*
	 * We'll need a shim like this in the SDK
	 */
	template <class T>
	class RequestCallback : virtual public IRequestCallback
	{
	public:
		RequestCallback(
			T^ target,
			void (T::*successCallback)(int, JSONObject),
			void (T::*errorCallback)(int, int, std::wstring) = nullptr,
			void (T::*statusCallback)(int, int, std::wstring) = nullptr) :
			_target(target),
			_successCallback(successCallback),
			_errorCallback(errorCallback),
			_statusCallback(statusCallback)
		{
			if (_target == nullptr) throw std::runtime_error("target may not be NULL");
			if (_successCallback == nullptr) throw std::runtime_error("successCallback may not be NULL");
		}

		void onSuccess(int id, JSONObject response)
		{
			(_target->*_successCallback)(id, response);
		}

		void onError(int id, int code, std::string message)
		{
			if (_errorCallback == nullptr) return;
			(_target->*_errorCallback)(id, code, std::wstring(message.begin(), message.end()));
		}

		void onStatus(int id, int code, std::string message)
		{
			if (_statusCallback == nullptr) return;
			(_target->*_statusCallback)(id, code, std::wstring(message.begin(), message.end()));
		}

	private:
		T^ _target;
		void (T::*_successCallback)(int, JSONObject);
		void (T::*_errorCallback)(int, int, std::wstring);
		void (T::*_statusCallback)(int, int, std::wstring);
	};
}

namespace UWP101
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();

	protected:
		void sendOpenRequest(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void responseTextUpdated(Windows::UI::Xaml::Controls::TextBox^ sender, Windows::UI::Xaml::Controls::TextBoxTextChangingEventArgs^ e);

	private:
		void onSuccess(int id, BranchIO::JSONObject response);
		void onStatus(int id, int code, std::wstring message);
		void onError(int id, int code, std::wstring message);

		void displayResponse(const std::wstring& header, const std::wstring& message);
		void scrollToBottom(Windows::UI::Xaml::Controls::TextBox^ textBox);

		~MainPage();
		BranchIO::Branch* m_branch;
		BranchIO::RequestCallback<MainPage> m_callback;
	};
}
