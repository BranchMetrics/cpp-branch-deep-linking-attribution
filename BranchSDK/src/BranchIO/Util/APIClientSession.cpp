// Copyright (c) 2019-21 Branch Metrics, Inc.

#include "APIClientSession.h"

#include <Poco/Exception.h>
#include <Poco/JSON/JSONException.h>
#include <Poco/Net/Context.h>
#include <Poco/Net/HTTPCredentials.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/NetException.h>
#include <Poco/Net/SSLManager.h>
#include <Poco/NullStream.h>
#include <Poco/StreamCopier.h>
#include <Poco/URI.h>

#include <string>

#include "BranchIO/Defines.h"
#include "BranchIO/IRequestCallback.h"
#include "BranchIO/Util/Log.h"

using namespace std;
using namespace Poco;
using namespace Poco::Net;

namespace BranchIO {

APIClientSession&
APIClientSession::instance() {
    // constructed the first time through
    static APIClientSession _session(BRANCH_IO_URL_BASE);
    return _session;
}

APIClientSession::APIClientSession(const std::string& urlBase) :
    HTTPSClientSession(URI(urlBase).getHost(),
    URI(urlBase).getPort(),
    Poco::Net::SSLManager::instance().defaultClientContext()),
    _urlBase(urlBase),
    _shuttingDown(false) {
}

void
APIClientSession::stop() {
    {
        Mutex::ScopedLock _l(_mutex);
        // Ignore subsequent calls
        if (_shuttingDown) return;

        _shuttingDown = true;
    }

    try {
        if (connected()) abort();
    }
    catch (NetException& /* e */) {
        // Don't report this. This can happen despite the condition above.
        // This is just a way of shutting down the socket and any outstanding
        // requests to the server to terminate the RequestManager's thread.
        // @todo(jdee): Improve on this.

        // cerr << "abort failure: " << e.message() << endl;
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

        HTTPRequest request(HTTPRequest::HTTP_POST, path, HTTPMessage::HTTP_1_1);
        string requestBody = jsonPayload.stringify();

        request.setContentLength(requestBody.length());
        request.setContentType("application/json; charset=utf-8");

        /* ----- Send the request and body ----- */

        // bail out immediately before and after any I/O, which can take
        // time
        sendRequest(request, requestBody);
        if (isShuttingDown()) return false;

        BRANCH_LOG_D("Request sent. Waiting for response.");

        /* ----- Wait for the response ----- */
        return processResponse(callback, result);
    }
    catch (const Poco::Exception& e) {
        if (isShuttingDown()) return false;
        string description(e.what());
        description += ": " + e.message();
        BRANCH_LOG_W("Request failed. " << description);
        callback.onStatus(0, 0, description);
    }
    return false;
}

void
APIClientSession::sendRequest(Poco::Net::HTTPRequest& request, const std::string& body) {
    BRANCH_LOG_D("Sending request to " << request.getURI());
    BRANCH_LOG_V("Event Payload: " << body);

    // blocking socket write
    ostream& os = sendRequest(request);
    if (isShuttingDown()) return;

    // blocking socket write
    os << body;
}

bool
APIClientSession::processResponse(IRequestCallback& callback, JSONObject& result) {
    HTTPResponse response;
    // blocking socket read
    istream& rs = receiveResponse(response);
    if (isShuttingDown()) return false;

    HTTPResponse::HTTPStatus status = response.getStatus();

    string requestId;
    try {
        requestId = response["X-Branch-Request-Id"];
    }
    catch (NotFoundException&) {
    }
    if (!requestId.empty()) {
        BRANCH_LOG_D("[" << requestId << "] " << status << " " << response.getReason());
    }
    else {
        BRANCH_LOG_D(status << " " << response.getReason());
    }

    // @todo(jdee): Fine-tune this success-failure determination
    if (status == HTTPResponse::HTTP_OK) {
        try {
            result = JSONObject::parse(rs);

            BRANCH_LOG_V("Response body: " << result.stringify());

            callback.onSuccess(0, result);

            return true;
        } catch (Poco::JSON::JSONException&) {
            // Parsing Error.
            // @todo(jdee): Return the error from the parseResults
            callback.onStatus(0, 0, "Error parsing result");
        }
    } else {
        /*
         * Report HTTP status != 200 as an error. Pass the status
         * code as the error argument.
         */
        callback.onStatus(0, status, response.getReason());
        if (isShuttingDown()) return false;

        // Read the output stream anyway. Don't bother parsing.
        NullOutputStream null;
        StreamCopier::copyStream(rs, null);

        if (status < HTTPResponse::HTTP_INTERNAL_SERVER_ERROR) {
            // We don't want to retry this.  Call the error handler and return "true" to indicate that this was handled.
            callback.onError(0, status, response.getReason());
            return true;
        }
    }

    return false;
}

}  // namespace BranchIO
