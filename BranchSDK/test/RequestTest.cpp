#include "MockClientSession.h"
#include "ResponseCounter.h"
#include <BranchIO/Event/StandardEvent.h>
#include <BranchIO/Internal/Request.h>

using namespace std;
using namespace BranchIO;
using namespace testing;

class RequestTest : public ::testing::Test
{
protected:
    ResponseCounter mCallback;
    MockClientSession mClientSession;
};

TEST_F(RequestTest, Send)
{
    StandardEvent event(StandardEvent::Type::PURCHASE);

    Request request;

    EXPECT_CALL(mClientSession, post("/v1/open", _, _)).Times(1).WillOnce(Return(true));

    request.send(Defines::REGISTER_OPEN, JSONObject(), mCallback, &mClientSession);
}
