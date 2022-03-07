// Copyright (c) 2019-21 Branch Metrics, Inc.


#include "Sleeper.h"
#include <mutex>
#include <chrono>

using namespace std;

namespace BranchIO {

Sleeper::~Sleeper() {
    wake();
    {
        /*
         * Lock and unlock the mutex one last time to give sleep() a chance
         * to read _woken and exit.
         */
        scoped_lock _l(_mutex);
    }
}

bool
Sleeper::sleep(int32_t milliseconds) {

    int32_t remaining(milliseconds);
    auto t_start = std::chrono::steady_clock::now();

    unique_lock _l(_mutex);
    // reset state after any previous sleep()
    _woken = false;
    
    while (remaining > 0 && !_woken) {
       
        std::chrono::duration<int, std::milli> remainingDuration(remaining);
        _condition.wait_for(_l, remainingDuration);
       
        auto t_now = std::chrono::steady_clock::now();
        double elapsed_time_ms = std::chrono::duration<double, std::milli>(t_now - t_start).count();
        remaining = milliseconds - static_cast<int32_t>(elapsed_time_ms);
    }

    return _woken;
}

void
Sleeper::wake() {
    scoped_lock _l(_mutex);
    _woken = true;
    _condition.notify_all();
}

bool
Sleeper::isWoken() const {
    scoped_lock _l(_mutex);
    return _woken;
}

}  // namespace BranchIO
