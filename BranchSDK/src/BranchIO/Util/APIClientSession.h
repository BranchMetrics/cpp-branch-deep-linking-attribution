// Copyright (c) 2019-21 Branch Metrics, Inc.

#ifndef BRANCHIO_UTIL_APICLIENTSESSION_H__
#define BRANCHIO_UTIL_APICLIENTSESSION_H__

#include <Poco/Mutex.h>
#include <string>
#include <winrt/Windows.Web.Http.Headers.h>

#include "BranchIO/fwd.h"
#include "IClientSession.h"

namespace BranchIO {

/**
 * (Internal)
 * @todo(jdee): Document
 */
class APIClientSession
    : public virtual IClientSession {
 public:
    /**
     * @return this instance
     * @todo(jdee): Document
     */
    static APIClientSession& instance();

    /**
     * Constructor.
     * @param urlBase Base endpoint for the session conversation.
     */
    explicit APIClientSession(const std::string& urlBase);

    /**
     * @return the urlBase.
     */
    std::string getUrlBase() const {
        Poco::Mutex::ScopedLock _l(_mutex);
        return _urlBase;
    }

    bool post(
        const std::string& path,
        const JSONObject& jsonPayload,
        IRequestCallback& callback,
        JSONObject& result);

    /**
     * @todo(jdee): Document
     */
    void stop();

    /**
     * Thread-safe method to determine if stop() has been called.
     * @return true if shutting down, false otherwise
     */
    bool isShuttingDown() const {
        Poco::Mutex::ScopedLock _l(_mutex);
        return _shuttingDown;
    }

    /**
     * Wait for and handle the response after sending a request.
     * @param callback callback for the response/error
     * @return true on success, false otherwise
     */
    bool processResponse(IRequestCallback& callback, JSONObject& result, winrt::Windows::Web::Http::HttpResponseMessage& httpResponseMessage);

 private:
    mutable Poco::Mutex _mutex;
    std::string _urlBase;
    bool volatile _shuttingDown;
    winrt::Windows::Web::Http::HttpClient _httpClient;
};

}  // namespace BranchIO

#endif  // BRANCHIO_UTIL_APICLIENTSESSION_H__
