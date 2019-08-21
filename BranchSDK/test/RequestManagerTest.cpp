// #include <iostream>
//
// #include <BranchIO/RequestManager.h>
//
// #include "MockClientSession.h"
// #include "MockRequest.h"
// #include "ResponseCounter.h"
// #include "Util.h"
//
// using namespace std;
// using namespace BranchIO;
// using namespace testing;
//
// /**
//  * Test case class
//  */
// class RequestManagerTest : public ::testing::Test
// {
// protected:
//     void SetUp()
//     {
//         mRequest1 = new MockRequest;
//         mRequest2 = new MockRequest;
//         mRequestManager = new RequestManager(&mClientSession);
//         mRequestManager->start();
//     }
//
//     void TearDown()
//     {
//         // destructor terminates background thread after each test case.
//         delete mRequestManager;
//
//         // Safest to delete these after the request manager.
//         // Hence we make them member vars instead of locals.
//         delete mRequest2;
//         delete mRequest1;
//
//         mRequestManager = NULL;
//         mRequest2 = mRequest1 = NULL;
//     }
//
//     MockClientSession mClientSession;
//     RequestManager* mRequestManager;
//     ResponseCounter mCallback;
//     MockRequest *mRequest1, *mRequest2;
// };
//
// // ----- Tests -----
//
// TEST_F(RequestManagerTest, Enqueue)
// {
//     IRequest::Ptr requestPtr(mRequest1);
//
//     // Expect the request to be sent by the background thread after it's
//     // enqueued.
//     EXPECT_CALL(*mRequest1, send(_)).Times(1).WillOnce(Invoke(respondToRequest));
//
//     mRequestManager->enqueue(requestPtr, &mCallback);
//
//     /*
//      * Wait till 1 response is received or 1 second passes. Report test failure
//      * if the request count doesn't match. (Might indicate a timeout.)
//      */
//     mCallback.waitForResponses(1, 1000);
//     ASSERT_EQ(1, mCallback.getResponseCount());
// }
//
// TEST_F(RequestManagerTest, QueueSequence)
// {
//     IRequest::Ptr requestPtr1(mRequest1), requestPtr2(mRequest2);
//
//     // Expect the following requests to be sent in order
//     InSequence seq;
//     EXPECT_CALL(*mRequest1, send(_)).Times(1).WillOnce(Invoke(respondToRequest));
//     EXPECT_CALL(*mRequest2, send(_)).Times(1).WillOnce(Invoke(respondToRequest));
//
//     mRequestManager->enqueue(requestPtr1, &mCallback);
//     mRequestManager->enqueue(requestPtr2, &mCallback);
//
//     /*
//      * Wait till 2 responses are received or 1 second passes. Report test failure
//      * if the request count doesn't match. (Might indicate a timeout.)
//      */
//     mCallback.waitForResponses(2, 1000);
//     ASSERT_EQ(2, mCallback.getResponseCount());
// }
//
// TEST_F(RequestManagerTest, DefaultCallback)
// {
//     mRequestManager->setDefaultCallback(&mCallback);
//
//     IRequest::Ptr requestPtr1(mRequest1), requestPtr2(mRequest2);
//
//     // Expect the following requests to be sent in order
//     InSequence seq;
//     EXPECT_CALL(*mRequest1, send(_)).Times(1).WillOnce(Invoke(respondToRequest));
//     EXPECT_CALL(*mRequest2, send(_)).Times(1).WillOnce(Invoke(respondToRequest));
//
//     mRequestManager->enqueue(requestPtr1);
//     mRequestManager->enqueue(requestPtr2);
//
//     /*
//      * Wait till 2 responses are received or 1 second passes. Report test failure
//      * if the request count doesn't match. (Might indicate a timeout.)
//      */
//     mCallback.waitForResponses(2, 1000);
//     ASSERT_EQ(2, mCallback.getResponseCount());
// }
