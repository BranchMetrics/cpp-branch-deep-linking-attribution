#include <string>

// Use <gtest/gtest.h> when not using mocks.
// Otherwise <gmock/gmock.h> also brings in gtest.
#include <gtest/gtest.h>

#include <BranchIO/LinkInfo.h>
#include <BranchIO/Event/Event.h>
#include <BranchIO/Util/Log.h>

using namespace BranchIO;
using namespace std;

class LinkInfoTest : public ::testing::Test {
};

TEST_F(LinkInfoTest, TestStringSetters) {
    LinkInfo info;

    // String Setters
    info.setAlias("My Alias")
        .setChannel("My Channel")
        .setFeature("My Feature")
        .setStage("My Stage")
        .setCampaign("My Campaign");

    std::string str = info.toString();
    ASSERT_GT(str.length(), 0);

    JSONObject::Ptr jsonObject = JSONObject::parse(str);
    ASSERT_GT(jsonObject->size(), 0);

    for (JSONObject::ConstIterator it = jsonObject->begin(); it != jsonObject->end(); ++it) {
        std::string value = it->second;

        ASSERT_STREQ("My", value.substr(0, 2).c_str());
    }

    // cout << "TestStringSetters\t" << jsonObject->stringify() << endl;
}

TEST_F(LinkInfoTest, TestIntegerSetters) {
    LinkInfo info;

    // Integer Setters
    info.setType(LinkInfo::LINK_TYPE_ONE_TIME_USE);
    info.setDuration(0xDEADBEEF);

    std::string str = info.toString();
    ASSERT_GT(str.length(), 0);

    JSONObject::Ptr jsonObject = JSONObject::parse(str);
    ASSERT_GT(jsonObject->size(), 0);

    for (JSONObject::ConstIterator it = jsonObject->begin(); it != jsonObject->end(); ++it) {
        int value = it->second;
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

    std::string str = info.toString();
    ASSERT_GT(str.length(), 0);

    JSONObject::Ptr jsonObject = JSONObject::parse(str);
    ASSERT_GT(jsonObject->size(), 0);

    // cout << "TestControlParams:\t" << str << endl;
}

// Test to emulate https://docs.branch.io/apps/deep-linking-api/#link-create
TEST_F(LinkInfoTest, TestLinkCreate) {
    LinkInfo info;

    info.addProperty("branch_key", "key_live_kaFuWw8WvY7yn1d9yYiP8gokwqjV0Swt");

    info.setChannel("facebook");
    info.setFeature("onboarding");
    info.setCampaign("new product");
    info.setStage("new user");

    info.addControlParameter("$canonical_identifier", "content/123");
    info.addControlParameter("$og_title", "Title from Deep Link");
    info.addControlParameter("$og_description", "Description from Deep Link");
    info.addControlParameter("$og_image_url", "http://www.lorempixel.com/400/400/");
    info.addControlParameter("$desktop_url", "http://www.example.com");

    info.addControlParameter("custom_integer", 1243);
    info.addControlParameter("custom_string", "everything");
    info.addControlParameter("custom_boolean", true);

    PropertyManager customObject;
    customObject.addProperty("random", "dictionary");
    info.addControlParameter("custom_object", customObject);

    std::string str = info.toString();
    ASSERT_GT(str.length(), 0);

    JSONObject::Ptr jsonObject = JSONObject::parse(str);
    ASSERT_GT(jsonObject->size(), 0);

    cout << "TestLinkCreate:\t" << str << endl;
}

class MyRequestCallback : public BranchIO::IRequestCallback {
protected:
    virtual void onSuccess(int id, BranchIO::JSONObject jsonResponse)
    {
        BRANCH_LOG_D("Callback Success!  Response: " << jsonResponse.stringify().c_str());
    }

    virtual void onError(int id, int error, std::string description)
    {
        BRANCH_LOG_D("Callback Error!" << description.c_str());
    }

    virtual void onStatus(int id, int error, std::string description)
    {
        BRANCH_LOG_D("Status Updated:" << description.c_str());
    }
};
#define BRANCH_KEY "key_live_efTsR1fbTucbHvX3N5RsOaamDtlPFLap"

TEST_F(LinkInfoTest, TestCreateLinkUrlRequest) {
    AppInfo _appInfo;
    _appInfo.setAppVersion("1.0")
        .setCountryCode("US")
        .setDeveloperIdentity("Branch Metrics")
        .setEnvironment("FULL_APP")
        .setLanguage("en");

//    Branch *_branchInstance = BranchIO::Branch::create(BRANCH_KEY, &_appInfo);
//    MyRequestCallback* _branchCallback = new MyRequestCallback;

    LinkInfo linkInfo;

    linkInfo.setFeature("testing");
    linkInfo.addControlParameter("extra_color", -6381877);

    //_branchInstance->sendEvent(linkInfo, _branchCallback);
}
