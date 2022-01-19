// Copyright (c) 2019-21 Branch Metrics, Inc.

#include "APIClientSession.h"
#include "StringUtils.h"
#include "BranchIO/Defines.h"
#include "BranchIO/IRequestCallback.h"
#include "BranchIO/Util/Log.h"

#include <string>
#include <winrt/Windows.Web.Http.Headers.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Security.Cryptography.h>

using namespace std;

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
        std::scoped_lock _l(_mutex);
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

        /* ----- Set up the HTTP request ----- */
    Uri uri{to_hstring(BRANCH_IO_URL_BASE), to_hstring(path) };

        // Construct the JSON to post.
        wstring requestBody = StringUtils::utf8_to_wstring(jsonPayload.stringify());
        HttpStringContent jsonContent( requestBody, UnicodeEncoding::Utf8, L"application/json");

        /* ----- Send the request and body ----- */

        // bail out immediately before and after any I/O, which can take
        // time - Post the JSON, and wait for a response.
    try
    {
        HttpResponseMessage  httpResponseMessage = _httpClient.PostAsync(uri, jsonContent).get();
        if (isShuttingDown()) return false;

        BRANCH_LOG_D("Request sent. Waiting for response.");

        // Make sure the post succeeded, and write out the response.
        bool responseCode = processResponse(callback, result, httpResponseMessage);

        httpResponseMessage.Close();

        return responseCode;
    }
    catch (winrt::hresult_error const& ex)
    {
        if (isShuttingDown()) return false;

        BRANCH_LOG_W("Request failed. " << to_string(ex.message()));
        callback.onStatus(0, 0, to_string(ex.message()));
    }
    
    return false;
}

bool
APIClientSession::processResponse(IRequestCallback& callback, JSONObject& result, HttpResponseMessage& httpResponseMessage) {

    if (isShuttingDown()) return false;

    HttpStatusCode  status = httpResponseMessage.StatusCode();
    
    std::string requestId;
    
    if(httpResponseMessage.Headers().HasKey(L"X-Branch-Request-Id"))
        requestId = to_string(httpResponseMessage.Headers().Lookup(L"X-Branch-Request-Id"));
    
    if (!requestId.empty()) {
        BRANCH_LOG_D("[" << requestId << "] " << (int)status << " " << to_string(httpResponseMessage.ReasonPhrase()));
    }
    else {
        BRANCH_LOG_D((int)status << " " << to_string(httpResponseMessage.ReasonPhrase()));
    }

    // @todo(jdee): Fine-tune this success-failure determination
    if (status == HttpStatusCode::Ok) {
        try {

            std::wstring  httpResponseBody = httpResponseMessage.Content().ReadAsStringAsync().get().c_str();
            result = JSONObject::parse(to_string(httpResponseBody));

            BRANCH_LOG_V("Response body: " << result.stringify());
            
            callback.onSuccess(0, result);

            return true;
        }
        catch (std::exception &) {
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
        callback.onStatus(0, (int)status, to_string(httpResponseMessage.ReasonPhrase()));
        if (isShuttingDown()) return false;

        if (status < HttpStatusCode::InternalServerError) {
            // We don't want to retry this.  Call the error handler and return "true" to indicate that this was handled.
            callback.onError(0, (int)status, to_string(httpResponseMessage.ReasonPhrase()));
            return true;
        }
    }

    return false;
}

}  // namespace BranchIO
