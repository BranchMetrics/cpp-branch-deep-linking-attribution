// Copyright (c) 2019 Branch Metrics, Inc.

#include "BranchIO/RequestManager.h"

#include <Poco/TaskNotification.h>
#include <cassert>

#include "BranchIO/IClientSession.h"
#include "BranchIO/Util/Log.h"

using namespace Poco;

namespace BranchIO {

RequestManager::RequestManager(IPackagingInfo& packagingInfo, IClientSession *clientSession) :
    _defaultCallback(NULL),
    _packagingInfo(&packagingInfo),
    _clientSession(clientSession),
    _shuttingDown(false),
    _currentRequest(NULL) {
}

RequestManager::~RequestManager() {
    // Make sure the thread is terminated before we exit.
    stop();
    waitTillFinished();
}

RequestManager&
RequestManager::setDefaultCallback(IRequestCallback* callback) {
    Mutex::ScopedLock _l(_mutex);
    _defaultCallback = callback;
    return *this;
}

IRequestCallback*
RequestManager::getDefaultCallback() const {
    Mutex::ScopedLock _l(_mutex);
    return _defaultCallback;
}

RequestManager& RequestManager::enqueue(
    const Event& event,
    IRequestCallback* callback,
    bool urgent) {
    // Make a copy of the Request on the heap. This will throw if both
    // callback and getDefaultCallback() are NULL.
    RequestTask* task = new RequestTask(*this, event, callback ? callback : getDefaultCallback());
    TaskNotification* notification = new TaskNotification(task);
    Notification::Ptr notificationPtr = Notification::Ptr(notification);

    if (urgent) {
        _queue.enqueueUrgentNotification(notificationPtr);
    } else {
        _queue.enqueueNotification(notificationPtr);
    }

    return *this;
}

void RequestManager::start() {
    // execute run() on this thread
    _thread.start(*this);
}

void RequestManager::stop() {
    if (getCurrentRequest()) getCurrentRequest()->cancel();
    if (getClientSession()) getClientSession()->stop();

    if (!_thread.isRunning()) return;

    // Stop the background thread
    {
        Poco::Mutex::ScopedLock _l(_mutex);
        _shuttingDown = true;
    }

    _queue.wakeUpAll();
}

void
RequestManager::waitTillFinished() {
    _thread.join();
}

bool RequestManager::isShuttingDown() const {
    Mutex::ScopedLock _l(_mutex);
    return _shuttingDown;
}

void RequestManager::run() {
    try {
        while (!isShuttingDown()) {
            Notification::Ptr notificationPtr = _queue.waitDequeueNotification();
            // wakeUpAll() from stop() will return us from here with or without
            // a notification.
            if (isShuttingDown()) {
                break;  // from while; fall through past exception handlers
            }

            // We have an indefinite wait, so the only way we can get here is
            // if we have a notification.
            Notification* notification = notificationPtr.get();
            assert(notification);

            // We get away here with static_cast knowing everything is a
            // TaskNotification. Otherwise, we'll need a dynamic_cast.
            TaskNotification* taskNotification = static_cast<TaskNotification*>(notification);
            Task* task = taskNotification->task();
            RequestTask* requestTask = static_cast<RequestTask*>(task);

            setCurrentRequest(&requestTask->getRequest());
            task->runTask();
            setCurrentRequest(NULL);
        }
    }
    catch (Poco::Exception& e) {
        // Poco exceptions
        BRANCH_LOG_E("Exception in RequestManager thread [" << e.what() << "]: " << e.message());
    }
    catch (std::exception& e) {
        // Other STL exceptions
        BRANCH_LOG_E("Exception in RequestManager thread: " << e.what());
    }
    catch (...) {
        // Anything else
        BRANCH_LOG_E("Unexpected exception in RequestManager thread.");
    }

    BRANCH_LOG_D("Terminating RequestManager thread.");
}

RequestManager::RequestTask::RequestTask(
    RequestManager& manager,
    const Event& event,
    IRequestCallback* callback) : Poco::Task("request"),
    _manager(manager),
    _event(event),
    _callback(callback) {
    if (!_callback) throw Poco::InvalidArgumentException("callback cannot be NULL.");
}

void
RequestManager::RequestTask::runTask() {
    JSONObject payload;
    _event.package(_manager.getPackagingInfo(), payload);

    // Send request synchronously
    // _clientSession may be passed in for testing. If not, we
    // create a real one here
    if (_manager.getClientSession()) {
        _request.send(_event.getAPIEndpoint(), payload, *_callback, _manager.getClientSession());
    } else {
        APIClientSession clientSession(BRANCH_IO_URL_BASE);
        _manager.setClientSession(&clientSession);
        _request.send(_event.getAPIEndpoint(), payload, *_callback, &clientSession);
        _manager.setClientSession(NULL);
    }
}

}  // namespace BranchIO
