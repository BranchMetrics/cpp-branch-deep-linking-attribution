#include "MockClientSession.h"
#include "ResponseCounter.h"
#include <BranchIO/Event/StandardEvent.h>
#include <BranchIO/Request.h>

using namespace std;
using namespace BranchIO;
using namespace testing;
/*
#include <gtest/gtest.h>
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Foundation.Collections.h>

#include <BranchIO/LinkInfo.h>
#include <BranchIO/Event/Event.h>

#include "ResponseCounter.h"
#include "MockClientSession.h"
#include "TestRequestCallback.h"
#include "Util.h"


using namespace winrt::Windows::Data::Json;
using namespace winrt::Windows::Foundation::Collections;

using namespace BranchIO;
//using namespace BranchIO::Test;
using namespace std;
using namespace testing;

*/
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

    EXPECT_CALL(mClientSession, post("https://api2.branch.io/v1/open", _, _, _)).Times(1).WillOnce(Return(true));

    request.send(Defines::REGISTER_OPEN, JSONObject(), mCallback, &mClientSession);
}
