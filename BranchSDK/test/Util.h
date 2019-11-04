#ifndef __BRANCHIO_TEST_UTIL_H__
#define __BRANCHIO_TEST_UTIL_H__

#include <BranchIO/Branch.h>

namespace BranchIO {
namespace Test {

/**
 * @return a key suitable for testing
 */
std::string getTestKey();

/**
* Utility function to invoke the callback when send is called on a mock.
* When calling:
*   EXPECT_CALL(*mRequest1, send(_)).Times(1).WillOnce(Invoke(respondToRequest));
* The argument to send (the callback, matched by the _) will be passed to this
* function.
*/
void respondToRequest(JSONObject &jsonResponse, IRequestCallback &callback);

/**
* Utility function to create a Branch Instance to be used for testing only.
* @return a new Branch Instance
*/
Branch *createTestInstance();

}  // namespace Test
}  // namespace BranchIO

#endif // __BRANCHIO_TEST_UTIL_H__
