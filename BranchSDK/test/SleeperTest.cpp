#include <BranchIO/Util/Sleeper.h>
#include <chrono>
#include <gmock/gmock.h>
#include <future>
#include <windows.h>

using namespace std;
using namespace BranchIO;
class WakeTimer  {
 public:
    WakeTimer(Sleeper& sleeper, int32_t sleepDuration) : 
        _sleeper(sleeper), _sleepDuration(sleepDuration) {
    
    }

    void start() {
        CreateThread(NULL, 0, threadFunc, (void*)this, 0, NULL);
    }

    static DWORD WINAPI threadFunc(void* Param){
        WakeTimer* This = (WakeTimer*)Param;
       // This->sleepAndWake();
        std::this_thread::sleep_for(std::chrono::milliseconds(This->_sleepDuration));
        This->_sleeper.wake();
        return 0;
    }

    void sleepAndWake(){
        std::this_thread::sleep_for(std::chrono::milliseconds(_sleepDuration));
        _sleeper.wake();
    }

 private:
    Sleeper& _sleeper;
    int _sleepDuration;
};

TEST(SleeperTest, Sleep) {
    Sleeper sleeper;
    
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
   
    /*
     * Go to sleep for 1 second. Verify that 1 second elapsed.
     */
    sleeper.sleep(1000);  // ms

    /*
     * Don't want an exact comparison here. Expected 1000 ms, has to be
     * at least 900.
     */
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    int timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    ASSERT_GE(timeElapsed, 900);  
}

TEST(SleeperTest, Wake) {
    Sleeper sleeper;
    WakeTimer timer(sleeper, 100);
    
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    timer.start();
    /*
     * Go to sleep for 1 second. Call wake() after 100 ms. Verify that
     * 100 ms elapsed.
     */
    sleeper.sleep(1000);  // ms

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    int timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    // Verify less than 1 second (exact comparison is OK, since it should be much less)
    ASSERT_LT(timeElapsed, 1000);  

    /*
     * Don't want an exact comparison here. Expected 100 ms, has to be
     * at least 90.
     */
    ASSERT_GE(timeElapsed, 90); 
}
