#ifndef __UTIL_H__
#define __UTIL_H__

#include <BranchIO/IRequestCallback.h>

/**
 * Utility function to invoke the callback when send is called on a mock.
 * When calling:
    EXPECT_CALL(*mRequest1, send(_)).Times(1).WillOnce(Invoke(respondToRequest));
 * The argument to send (the callback, matched by the _) will be passed to this
 * function.
 */
void respondToRequest(BranchIO::IRequestCallback& callback)
{
    BranchIO::JSONObject::Ptr response(new BranchIO::JSONObject);
    callback.onSuccess(0, response);
}

#endif // __UTIL_H__
