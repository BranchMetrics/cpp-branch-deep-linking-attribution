#ifndef __BRANCHIO_TEST_REQUESTCALLBACK_H__
#define __BRANCHIO_TEST_REQUESTCALLBACK_H__

#include <BranchIO/Branch.h>
#include <BranchIO/IRequestCallback.h>
#include <BranchIO/Util/Log.h>


namespace BranchIO {
namespace Test {

    /**
    * Basic Request Callback class to simply log responses.
    */
    class TestRequestCallback : public BranchIO::IRequestCallback {
    protected:
        virtual void onSuccess(int id, BranchIO::JSONObject jsonResponse) {
            BRANCH_LOG_D("Callback Success!  Response: " << jsonResponse.stringify().c_str());
        }

        virtual void onError(int id, int error, std::string description) {
            BRANCH_LOG_D("Callback Error!" << description.c_str());
        }

        virtual void onStatus(int id, int error, std::string description) {
            BRANCH_LOG_D("Status Updated:" << description.c_str());
        }
    };

}  // namespace Test
}  // namespace BranchIO

#endif // __BRANCHIO_TEST_REQUESTCALLBACK_H__
