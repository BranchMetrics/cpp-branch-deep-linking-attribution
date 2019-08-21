// Copyright (c) 2019 Branch Metrics, Inc.

#ifndef BRANCHIO_APICLIENTSESSION_H__
#define BRANCHIO_APICLIENTSESSION_H__

#include <Poco/Mutex.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <string>

#include "BranchIO/fwd.h"
#include "BranchIO/Defines.h"
#include "BranchIO/IClientSession.h"

namespace BranchIO {

/**
 * (Internal)
 * @todo(jdee): Document
 */
class APIClientSession
    : public Poco::Net::HTTPSClientSession,
      public virtual IClientSession {
 public:
    /**
     * @return SSL session context
     * @todo(jdee): Document
     */
    static Poco::Net::Context::Ptr getContext();

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
        IRequestCallback& callback);

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
     * Convenience method to send a request with its body.
     * @param request A request to send
     * @param body The body for the request
     */
    void sendRequest(Poco::Net::HTTPRequest& request, const std::string& body);
    using Poco::Net::HTTPSClientSession::sendRequest;

    /**
     * Wait for and handle the response after sending a request.
     * @param callback callback for the response/error
     * @return true on success, false otherwise
     */
    bool processResponse(IRequestCallback& callback);

 private:
    mutable Poco::Mutex _mutex;
    std::string _urlBase;
    bool volatile _shuttingDown;
};

}  // namespace BranchIO

#endif  // BRANCHIO_APICLIENTSESSION_H__
