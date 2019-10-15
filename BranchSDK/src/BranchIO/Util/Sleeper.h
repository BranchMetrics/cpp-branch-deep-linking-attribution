// Copyright (c) 2019 Branch Metrics, Inc.

#ifndef BRANCHIO_UTIL_SLEEPER_H__
#define BRANCHIO_UTIL_SLEEPER_H__

#include <Poco/Condition.h>
#include <Poco/Mutex.h>

#include <cstdint>

namespace BranchIO {

/**
 * Thread-safe wakeable class to delay execution.
   ```
   #include <BranchIO/Sleeper.h>
   BranchIO::Sleeper sleeper;

   // thread A:
   sleeper.sleep(10000); // ms

   // thread B:
   sleeper.wake(); // causes sleep() to return early
   ```
 */
class Sleeper {
 public:
     /**
      * Destructor
      */
     ~Sleeper();

     /**
      * Delay execution on this thread by the specified number of milliseconds.
      * May return early if wake() is called on another thread. Each call to
      * sleep() resets the woken state.
      * @param milliseconds sleep duration
      * @return true if woken by wake(); false if time expired
      */
     bool sleep(int32_t milliseconds);

     /**
      * Cause sleep() in any other thread to return immediately.
      */
     void wake();

     /**
      * Determine if this Sleeper has been woken since the last call to sleep().
      * @return true if woken, false otherwise
      */
     bool isWoken() const;

 private:
    Poco::Condition mutable _condition;
    Poco::Mutex mutable _mutex;
    bool volatile _woken;
};

}  // namespace BranchIO

#endif  // BRANCHIO_UTIL_SLEEPER_H__
