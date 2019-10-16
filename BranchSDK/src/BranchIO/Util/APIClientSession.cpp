// Copyright (c) 2019 Branch Metrics, Inc.

#include "APIClientSession.h"

#include <Poco/Exception.h>
#include <Poco/Net/Context.h>
#include <Poco/Net/HTTPCredentials.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/NetException.h>
#include <Poco/NullStream.h>
#include <Poco/StreamCopier.h>
#include <Poco/URI.h>

#include <string>

#include "BranchIO/Defines.h"
#include "BranchIO/IRequestCallback.h"
#include "Log.h"

using namespace std;
using namespace Poco;
using namespace Poco::Net;

namespace BranchIO {

Poco::Net::Context::Ptr APIClientSession::getContext() {
    return new Context
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
        // Unix / Mac
        (Context::CLIENT_USE, "", "", "",
         Context::VERIFY_NONE, 9, false,
         "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
#else
        // Windows
        (Context::CLIENT_USE, "");
#endif
}

APIClientSession&
APIClientSession::instance() {
    // constructed the first time through
    static APIClientSession _session(BRANCH_IO_URL_BASE);
    return _session;
}

APIClientSession::APIClientSession(const std::string& urlBase) :
    HTTPSClientSession(URI(urlBase).getHost(),
    URI(urlBase).getPort(),
    getContext()),
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
    IRequestCallback& callback) {
    if (isShuttingDown()) return false;

    try {
        /* ----- Set up the HTTP request ----- */

        HTTPRequest request(HTTPRequest::HTTP_POST, path, HTTPMessage::HTTP_1_1);
        string requestBody = jsonPayload.stringify();

        request.setContentLength(requestBody.length());
        request.setContentType("application/json; charset=utf-8");

        /* ----- Send the request and body ----- */

        // bail out here and below immediately after any I/O, which can take
        // time
        sendRequest(request, requestBody);
        if (isShuttingDown()) return false;

        BRANCH_LOG_D("Request sent. Waiting for response.");

        /* ----- Wait for the response ----- */
        return processResponse(callback);
    }
    catch (const Poco::Exception& e) {
        if (isShuttingDown()) return false;
        callback.onStatus(0, 0, string(e.what()) + ": " + e.message());
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
APIClientSession::processResponse(IRequestCallback& callback) {
    HTTPResponse response;
    // blocking socket read
    istream& rs = receiveResponse(response);
    if (isShuttingDown()) return false;

    HTTPResponse::HTTPStatus status = response.getStatus();
    BRANCH_LOG_D(status << " " << response.getReason());

    // @todo(jdee): Fine-tune this success-failure determination
    if (status == 200) {
        JSONObject::Ptr ptr = JSONObject::parse(rs);
        if (ptr) {
            callback.onSuccess(0, *ptr);
            return true;
        } else {
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
    }

    return false;
}

}  // namespace BranchIO
