// Copyright (c) 2019 Branch Metrics, Inc.

#include <Poco/Clock.h>

#include "BranchIO/Sleeper.h"

using namespace std;
using namespace Poco;

namespace BranchIO {

Sleeper::~Sleeper() {
    wake();
    {
        /*
         * Lock and unlock the mutex one last time to give sleep() a chance
         * to read _woken and exit.
         */
        Mutex::ScopedLock _l(_mutex);
    }
}

bool
Sleeper::sleep(int32_t milliseconds) {
    Clock start;
    int32_t remaining(milliseconds);

    Mutex::ScopedLock _l(_mutex);
    // reset state after any previous sleep()
    _woken = false;

    while (remaining > 0 && !_woken) {
        _condition.tryWait(_mutex, remaining);
        remaining = milliseconds - static_cast<int32_t>(start.elapsed() * 1.e-3);
    }

    return _woken;
}

void
Sleeper::wake() {
    Mutex::ScopedLock _l(_mutex);
    _woken = true;
    _condition.broadcast();
}

bool
Sleeper::isWoken() const {
    Mutex::ScopedLock _l(_mutex);
    return _woken;
}

}  // namespace BranchIO
