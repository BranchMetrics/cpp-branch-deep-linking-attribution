#include <string>

// Use <gtest/gtest.h> when not using mocks.
// Otherwise <gmock/gmock.h> also brings in gtest.
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
using namespace BranchIO::Test;
using namespace std;
using namespace testing;

class LinkInfoTest : public ::testing::Test {
protected:
    void SetUp() {
        mBranch = createTestInstance();
    }

    void TearDown() {
        delete mBranch;
    }

    ResponseCounter mCallback;
    MockClientSession mClientSession;
    Branch* mBranch;
};

TEST_F(LinkInfoTest, TestStringSetters) {
    LinkInfo info;

    // String Setters
    info.setAlias("My Alias")
        .setChannel("My Channel")
        .setFeature("My Feature")
        .setStage("My Stage")
        .setCampaign("My Campaign");

    string str = info.toString();
    ASSERT_GT(str.length(), 0);

    JSONObject jsonObject = JSONObject::parse(str);
    ASSERT_GT(jsonObject.size(), 0);

    // Copy the key/values
    IIterator<IKeyValuePair<winrt::hstring, IJsonValue>> it;
    JsonObject sourceJObject = jsonObject.getWinRTJsonObj();

    for (it = sourceJObject.First(); it.HasCurrent(); it.MoveNext()) {
        std::string value = to_string(it.Current().Value().GetString());
        ASSERT_STREQ("My", value.substr(0, 2).c_str());
    }
}

TEST_F(LinkInfoTest, TestIntegerSetters) {
    LinkInfo info;

    // Integer Setters
    info.setType(LinkInfo::LINK_TYPE_ONE_TIME_USE);
    info.setDuration(0xDEADBEEF);

    string str = info.toString();
    ASSERT_GT(str.length(), 0);

    JSONObject jsonObject = JSONObject::parse(str);
    ASSERT_GT(jsonObject.size(), 0);

    IIterator<IKeyValuePair<winrt::hstring, IJsonValue>> it;
    JsonObject sourceJObject = jsonObject.getWinRTJsonObj();

    for (it = sourceJObject.First(); it.HasCurrent(); it.MoveNext()) {
        int value = (it.Current().Value().GetNumber());
        ASSERT_NE(value, 0);
    }
}

TEST_F(LinkInfoTest, TestControlParams) {
    LinkInfo info;
    info.addControlParameter("PARAM1", "VALUE1");
    info.addControlParameter("PARAM2", "VALUE2");
    info.addControlParameter("PARAM3", "VALUE3");
    info.addControlParameter("PARAM4", "VALUE4");
    info.addControlParameter("PARAM5", "VALUE5");

    string str = info.toString();
    ASSERT_GT(str.length(), 0);

    JSONObject jsonObject = JSONObject::parse(str);
    ASSERT_GT(jsonObject.size(), 0);
}

TEST_F(LinkInfoTest, TestTagParams) {
    LinkInfo info;
    info.addTag("TAG1");
    info.addTag("TAG2");
    info.addTag("TAG3");
    info.addTag("TAG4");
    info.addTag("TAG5");

    string str = info.toString();

    ASSERT_GT(str.length(), 0);

    JSONObject jsonObject = JSONObject::parse(str);
    ASSERT_GT(jsonObject.size(), 0);
}

TEST_F(LinkInfoTest, CreateUrl) {
    LinkInfo info;
    info.setClientSession(&mClientSession);

    EXPECT_CALL(mClientSession, post("/v1/url", _, _, _)).Times(1).WillOnce(Return(true));

    info.createUrl(mBranch, &mCallback);
}

TEST_F(LinkInfoTest, FallbackToLongUrl) {
    /*
     * Probably as easy to do this explicitly as with gmock
     */
    struct FailingClientSession : public virtual IClientSession {
        void stop() {}
        bool post(const string& path, const JSONObject& payload, IRequestCallback& callback, JSONObject& result) {
            callback.onError(0, 0, "something happened");
            return true;
        }
    } failingClientSession;

    struct MockCallback : public IRequestCallback {
        MOCK_METHOD2(onSuccess, void(int, JSONObject));
        MOCK_METHOD3(onStatus, void(int, int, string));
        MOCK_METHOD3(onError, void(int, int, string));
    } callback;

    LinkInfo info;
    info.setClientSession(&failingClientSession);

    EXPECT_CALL(callback, onSuccess(_, _)).Times(1);

    info.createUrl(mBranch, &callback);
}