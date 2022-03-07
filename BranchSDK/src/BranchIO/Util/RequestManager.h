// Copyright (c) 2019-21 Branch Metrics, Inc.

#ifndef BRANCHIO_UTIL_REQUESTMANAGER_H__
#define BRANCHIO_UTIL_REQUESTMANAGER_H__

#include "APIClientSession.h"
#include "BranchIO/Event/Event.h"
#include "BranchIO/fwd.h"
#include "BranchIO/Request.h"
#include <deque>

namespace BranchIO {

/**
 * (Internal) Thread-safe request manager class with an optional background thread
 * that manages a priority request queue with callbacks.
 */
class BRANCHIO_DLL_EXPORT RequestManager {
 public:
    /**
     * Constructor.
     * @param packagingInfo reference to a source of packaging information
     * @param clientSession existing client session, or NULL to start a new session
     */
    explicit RequestManager(IPackagingInfo& packagingInfo, IClientSession* clientSession = nullptr);
    ~RequestManager();

    /**
     * Set a default callback to use for requests that don't provide a
     * callback via enqueue. Initialized to NULL. Can be reset to NULL
     * as well.
     * @param callback pointer to a callback to use
     * @return a reference to the RequestManager
     */
    RequestManager& setDefaultCallback(IRequestCallback* callback);

    /**
     * @return a pointer to the default callback instead of NULL.
     */
    IRequestCallback* getDefaultCallback() const;

    /**
     * Insert a request at the back of the queue with an optional callback.
     * If callback is NULL, the
     * Requests are processed in insertion order. However if the optional urgent
     * flag is set, the request is inserted at the front of the queue.
     *
     * @param event Event to send
     * @param callback (optional) Interface for success and failure response.
     * @param urgent (optional) if true, the request is inserted at the front of the queue instead of the back.
     * @return a reference to the RequestManager
     * @throw std::exception - InvalidArgumentException if callback and the default callback are both NULL
     */
    RequestManager& enqueue(
        const BaseEvent& event,
        IRequestCallback* callback = nullptr,
        bool urgent = false);

    /**
     * Start(create) the request manager's background thread.
     */
    void start();

    /**
     *  Request manager's background thread executes this method when its started.
     *  It dequeues RequestTask from queue and executes them one by one.
     */
    void run();

    /**
     * Stop the request manager's background thread and join the worker thread.
     * Automatically called from the destructor. Safe to call explicitly as
     * well.
     */
    void stop();

    /**
     * Block until the background thread has terminated. The destructor also
     * blocks.
     */
    void waitTillFinished();

    /**
     * @return true if this request manager is shutting down.
     */
    bool isShuttingDown() const;

 protected:
    /**
     * (Unimplemented) Copy constructor.
     * @param other Other
     * @todo(jdee) Revisit
     */
    RequestManager(const RequestManager& other) {}

    /**
     * (Unimplemented) Equals operator.
     * @param other Other
     * @return a reference to the RequestManager
     * @todo(jdee) Revisit)
     */
    RequestManager& operator=(const RequestManager& other) { return *this; }

    /**
     * Shim object for TaskQueue. 
     * Just contains the request and callback and sends the request
     * synchronously via runTask().
     */
    struct RequestTask {
        /**
         * Constructor.
         * @param manager A reference to the RequestManager that enqueued this
         * @param event Event to send
         * @param callback Interface for success and failure response.
         */
        RequestTask(RequestManager& manager, const BaseEvent& event, IRequestCallback* callback);

        /**
         * Task Runner
         */
        void runTask();

        /**
         * Get a reference to the Request object. Used to terminate retry loops
         * on shutdown.
         * @return a reference to the associated Request
         */
        Request& getRequest() { return _request; }

        /**
         * Get a reference to the Request object. Used to terminate retry loops
         * on shutdown.
         * @return a reference to the associated Request
         */
        Request const& getRequest() const { return _request; }

     private:
        RequestManager& _manager;
        Request _request;
        BaseEvent _event;
        IRequestCallback* _callback;
    };

    /**
    * Inserts a RequestTask at the back of the queue and notifies about it.
    * @param task RequestTask to be added.
    */
    void enqueueTask(RequestTask* task);
    
    /**
    * Inserts a RequestTask in the front of the queue and notifies about it.
    * @param task RequestTask to be added.
    */
    void enqueueUrgentTask(RequestTask* task);
    
    /**
     * Pops and returns a RequestTask in the front of the queue. 
     * If queue is empty, it waits until any RequestTask in enqueued.
     * @return task RequestTask in the front of the queue.
     */
    RequestTask* waitDequeueNotification();

    /**
     * Wakes up all threads that wait for a task.
     */
    void wakeUpAll();

    /**
     * Synchronized getter for _clientSession. Will usually only
     * be non-NULL in unit tests.
     * @return pointer to an IClientSession or NULL
     */
    IClientSession *getClientSession() const {
        std::scoped_lock _l(_mutex);
        return _clientSession;
    }

    /**
     * Synchronized setter for _clientSession. Called from the
     * constructor in unit tests.
     * @param clientSession pointer to an IClientSession to use
     */
    void setClientSession(IClientSession *clientSession) {
        std::scoped_lock _l(_mutex);
        _clientSession = clientSession;
    }

    /// Allow RequestTask to call protected and private methods
    friend struct RequestTask;

    /**
     * Gets a pointer to the current outstanding Request, if any
     * @return the current Request or NULL if none
     */
    Request* getCurrentRequest() const {
        std::scoped_lock _l(_mutex);
        return _currentRequest;
    }

    /**
     * Sets the current request.
     * @param request the current Request or NULL
     * @return *this
     */
    RequestManager& setCurrentRequest(Request* request) {
        std::scoped_lock _l(_mutex);
        _currentRequest = request;
        return *this;
    }

    /**
     * Packaging info getter
     * @return the IPackagingInfo
     */
    IPackagingInfo& getPackagingInfo() { return *_packagingInfo; }

 private:
    std::deque<RequestTask *>  _queue;
    mutable std::mutex _mutex;
    std::condition_variable mutable _available;
    std::thread _thread;
    IRequestCallback* volatile _defaultCallback;
    IPackagingInfo* volatile _packagingInfo;
    IClientSession* volatile _clientSession;
    bool volatile _shuttingDown;
    Request* volatile _currentRequest;
};

}  // namespace BranchIO

#endif  // BRANCHIO_UTIL_REQUESTMANAGER_H__
