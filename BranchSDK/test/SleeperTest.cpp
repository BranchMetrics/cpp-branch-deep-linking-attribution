#include <BranchIO/Sleeper.h>
#include <gmock/gmock.h>
#include <Poco/Clock.h>
#include <Poco/Timer.h>

using namespace std;
using namespace BranchIO;
using namespace Poco;

class WakeTimer : public Timer {
 public:
    WakeTimer(Sleeper& sleeper, int32_t delay) :
        Timer(delay, 0),  // fire once after delay ms
        _sleeper(sleeper) {}

    void start() {
        TimerCallback<WakeTimer> callback(*this, &WakeTimer::onTimer);
        Timer::start(callback);
    }

    void onTimer(Timer& timer) {
        _sleeper.wake();
    }

 private:
    Sleeper& _sleeper;
};

TEST(SleeperTest, Sleep) {
    Sleeper sleeper;
    Clock start;

    /*
     * Go to sleep for 1 second. Verify that 1 second elapsed.
     */
    sleeper.sleep(1000);  // ms

    /*
     * Don't want an exact comparison here. Expected 1000 ms, has to be
     * at least 900.
     */
    ASSERT_GE(start.elapsed(), 9.e5);  // us
}

TEST(SleeperTest, Wake) {
    Sleeper sleeper;
    WakeTimer timer(sleeper, 100);
    Clock start;

    /*
     * Go to sleep for 1 second. Call wake() after 100 ms. Verify that
     * 100 ms elapsed.
     */
    timer.start();
    sleeper.sleep(1000);  // ms

    // Verify less than 1 second (exact comparison is OK, since it should be much less)
    ASSERT_LT(start.elapsed(), 1.e6);  // us

    /*
     * Don't want an exact comparison here. Expected 100 ms, has to be
     * at least 90.
     */
    ASSERT_GE(start.elapsed(), 9.e4);  // us
}
