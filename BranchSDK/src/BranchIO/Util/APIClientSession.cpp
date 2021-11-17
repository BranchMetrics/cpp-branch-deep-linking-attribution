// Copyright (c) 2019-21 Branch Metrics, Inc.

#include "APIClientSession.h"
#include "StringUtils.h"
#include "BranchIO/Defines.h"
#include "BranchIO/IRequestCallback.h"
#include "BranchIO/Util/Log.h"

#include <Poco/JSON/JSONException.h>

#include <string>

#include <winrt/Windows.Web.Http.Headers.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Security.Cryptography.h>

using namespace std;
using namespace Poco;

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Web::Http;
using namespace winrt::Windows::Web::Http::Headers;
using namespace winrt::Windows::Security::Credentials;
using namespace winrt::Windows::Storage::Streams;

namespace BranchIO {

APIClientSession&
APIClientSession::instance() {
    // constructed the first time through
    static APIClientSession _session(BRANCH_IO_URL_BASE);
    return _session;
}

APIClientSession::APIClientSession(const std::string& urlBase) :_urlBase(urlBase),
    _shuttingDown(false) {
    init_apartment();
    _httpClient = HttpClient();
}

void
APIClientSession::stop() {
    {
        Mutex::ScopedLock _l(_mutex);
        // Ignore subsequent calls
        if (_shuttingDown) return;

        _shuttingDown = true;
    }
}

bool
APIClientSession::post(
    const std::string& path,
    const JSONObject& jsonPayload,
    IRequestCallback& callback,
    JSONObject& result) {
    if (isShuttingDown()) return false;

    try {
        /* ----- Set up the HTTP request ----- */

        Uri uri{ StringUtils::utf8_to_wstring(BRANCH_IO_URL_BASE), StringUtils::utf8_to_wstring(path) };
       
        // Construct the JSON to post.
        wstring requestBody = StringUtils::utf8_to_wstring(jsonPayload.stringify());
        HttpStringContent jsonContent( requestBody, UnicodeEncoding::Utf8, L"application/json");

        /* ----- Send the request and body ----- */

        // bail out immediately before and after any I/O, which can take
        // time - Post the JSON, and wait for a response.

        HttpResponseMessage  httpResponseMessage = _httpClient.PostAsync( uri, jsonContent).get();

        // Make sure the post succeeded, and write out the response.
        httpResponseMessage.EnsureSuccessStatusCode();

        /* ----- Wait for the response ----- */
        return processResponse(callback, result, httpResponseMessage);
    }
    catch (winrt::hresult_error const& ex) {
        if (isShuttingDown()) return false;
        string description( to_string(ex.code()));
        description += ": " + StringUtils::wstring_to_utf8(ex.message().c_str());
        BRANCH_LOG_W("Request failed. " << description);
        callback.onStatus(0, 0, description);
    }
    return false;
}

bool
APIClientSession::processResponse(IRequestCallback& callback, JSONObject& result, HttpResponseMessage& httpResponseMessage) {

    if (isShuttingDown()) return false;

    HttpStatusCode  status = httpResponseMessage.StatusCode();
    
    std::string requestId;
    
    try {
        requestId = StringUtils::wstring_to_utf8(httpResponseMessage.Headers().Lookup(L"X-Branch-Request-Id").c_str());
    }
    catch (NotFoundException&) {
    }
    if (!requestId.empty()) {
        BRANCH_LOG_D("[" << requestId << "] " << (int)status << " " << StringUtils::wstring_to_utf8(httpResponseMessage.ReasonPhrase().c_str()));
    }
    else {
        BRANCH_LOG_D((int)status << " " << StringUtils::wstring_to_utf8(httpResponseMessage.ReasonPhrase().c_str()));
    }

    // @todo(jdee): Fine-tune this success-failure determination
    if (status == HttpStatusCode::Ok) {
        try {

            std::wstring  httpResponseBody = httpResponseMessage.Content().ReadAsStringAsync().get().c_str();
            result = JSONObject::parse(StringUtils::wstring_to_utf8(httpResponseBody));

            BRANCH_LOG_V("Response body: " << result.stringify());

            callback.onSuccess(0, result);

            return true;
        }
        catch (Poco::JSON::JSONException&) {
            // Parsing Error.
            // @todo(jdee): Return the error from the parseResults
            callback.onStatus(0, 0, "Error parsing result");
        }
    }
    else {
        /*
         * Report HTTP status != 200 as an error. Pass the status
         * code as the error argument.
         */
        callback.onStatus(0, (int)status, StringUtils::wstring_to_utf8(httpResponseMessage.ReasonPhrase().c_str()));
        if (isShuttingDown()) return false;

        if (status < HttpStatusCode::InternalServerError) {
            // We don't want to retry this.  Call the error handler and return "true" to indicate that this was handled.
            callback.onError(0, (int)status, StringUtils::wstring_to_utf8(httpResponseMessage.ReasonPhrase().c_str()));
            return true;
        }
    }

    return false;
}

}  // namespace BranchIO
