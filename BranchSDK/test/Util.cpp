#ifndef __BRANCHIO_TEST_UTIL_H__
#define __BRANCHIO_TEST_UTIL_H__

#include "Util.h"

#include <BranchIO/Branch.h>
#include <BranchIO/IRequestCallback.h>
#include <BranchIO/Util/Log.h>

#define TEST_KEY "key_live_efTsR1fbTucbHvX3N5RsOaamDtlPFLap"

namespace BranchIO {
namespace Test {

    /**
     * @return a key suitable for testing
     */
    std::string getTestKey() {
        return TEST_KEY;
    }

    /**
    * Utility function to invoke the callback when send is called on a mock.
    * When calling:
    *   EXPECT_CALL(*mRequest1, send(_)).Times(1).WillOnce(Invoke(respondToRequest));
    * The argument to send (the callback, matched by the _) will be passed to this
    * function.
    */
    void respondToRequest(JSONObject &jsonResponse, IRequestCallback &callback) {
        callback.onSuccess(0, jsonResponse);
    }

    /**
    * Utility function to create a Branch Instance to be used for testing only.
    * @return a new Branch Instance
    */
    Branch *createTestInstance() {
        AppInfo _appInfo;
        _appInfo.setAppVersion("1.0")
        .setCountryCode("US")
        .setDeveloperIdentity("Branch Metrics")
        .setEnvironment("TEST_APP")
        .setLanguage("en");

        return BranchIO::Branch::create(TEST_KEY, &_appInfo);
    }

}  // namespace Test
}  // namespace BranchIO

#endif // __BRANCHIO_TEST_UTIL_H__
