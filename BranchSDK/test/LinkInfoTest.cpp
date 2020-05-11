#include <string>

// Use <gtest/gtest.h> when not using mocks.
// Otherwise <gmock/gmock.h> also brings in gtest.
#include <gtest/gtest.h>

#include <BranchIO/LinkInfo.h>
#include <BranchIO/Event/Event.h>
#include <Poco/Base64Encoder.h>

#include "TestRequestCallback.h"
#include "Util.h"

using namespace BranchIO;
using namespace std;

class LinkInfoTest : public ::testing::Test {
};

/**
 * Initialize a "typical" branch link object with some basic values.
 * @param info LinkInfo object to fill
 */
static void initTestLink(LinkInfo &info) {
    info.addTag("Tag1");
    info.addTag("Tag2");

    info.setAlias("my alias");
    info.setChannel("facebook");
    info.setFeature("onboarding");
    info.setCampaign("new product");
    info.setStage("new user");

    info.addControlParameter("$canonical_identifier", "content/123");
    info.addControlParameter("$og_title", "Title from Deep Link");
    info.addControlParameter("$og_description", "Description from Deep Link");
    info.addControlParameter("$og_image_url", "http://www.lorempixel.com/400/400/");
    info.addControlParameter("$desktop_url", "http://www.example.com");

    info.addControlParameter("custom_string", "everything");
    info.addControlParameter("custom_integer", 1243);
    info.addControlParameter("custom_boolean", true);

    PropertyManager customObject;
    customObject.addProperty("random", "dictionary");
    info.addControlParameter("custom_object", customObject);
}

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

    JSONObject jsonObject = JSONObject::parse(str);
    ASSERT_GT(jsonObject.size(), 0);

    for (JSONObject::ConstIterator it = jsonObject.begin(); it != jsonObject.end(); ++it) {
        std::string value = it->second;

        ASSERT_STREQ("My", value.substr(0, 2).c_str());
    }

    info.cancel();
}

TEST_F(LinkInfoTest, TestIntegerSetters) {
    LinkInfo info;

    // Integer Setters
    info.setType(LinkInfo::LINK_TYPE_ONE_TIME_USE);
    info.setDuration(0xDEADBEEF);

    std::string str = info.toString();
    ASSERT_GT(str.length(), 0);

    JSONObject jsonObject = JSONObject::parse(str);
    ASSERT_GT(jsonObject.size(), 0);

    for (JSONObject::ConstIterator it = jsonObject.begin(); it != jsonObject.end(); ++it) {
        int value = it->second;
        ASSERT_NE(value, 0);
    }

    info.cancel();
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

    JSONObject jsonObject = JSONObject::parse(str);
    ASSERT_GT(jsonObject.size(), 0);


    info.cancel();
}

TEST_F(LinkInfoTest, TestTagParams) {
    LinkInfo info;
    info.addTag("TAG1");
    info.addTag("TAG2");
    info.addTag("TAG3");
    info.addTag("TAG4");
    info.addTag("TAG5");

    std::string str = info.toString();

    ASSERT_GT(str.length(), 0);

    JSONObject jsonObject = JSONObject::parse(str);
    ASSERT_GT(jsonObject.size(), 0);


    info.cancel();
}

// Test to emulate https://docs.branch.io/apps/deep-linking-api/#link-create
TEST_F(LinkInfoTest, TestLinkCreate) {
    LinkInfo info;
    initTestLink(info);

    std::string str = info.toString();
    ASSERT_GT(str.length(), 0);

    JSONObject jsonObject = JSONObject::parse(str);
    ASSERT_GT(jsonObject.size(), 0);

    info.cancel();
}

// Test to create a "Long Link Url" from a LinkInfo
TEST_F(LinkInfoTest, TestLongLinkCreate) {
    Branch *_branchInstance = BranchIO::Test::createTestInstance();
    LinkInfo linkInfo;
    initTestLink(linkInfo);

    std::string longUrl = linkInfo.createLongUrl(_branchInstance);

    linkInfo.cancel();
}


// The implementation here is commented out so as to not create a new link every time the unit test suite runs.
// To debug short link creation, uncomment as needed.
TEST_F(LinkInfoTest, TestCreateLinkUrlRequest) {
    // Branch *_branchInstance = BranchIO::Test::createTestInstance();
    // IRequestCallback* _branchCallback = new BranchIO::Test::TestRequestCallback;

    LinkInfo linkInfo;

    linkInfo.setFeature("testing");
    linkInfo.addControlParameter("extra_color", -6381877);

    linkInfo.cancel();
}

// We are taking advantage here of the fact that when tracking is disabled,
// the implementation will short circuit and create a long link.
TEST_F(LinkInfoTest, TestCreateLinkUrlFallback) {
    Branch *branchInstance = BranchIO::Test::createTestInstance();
    IRequestCallback* branchCallback = new BranchIO::Test::TestRequestCallback;

    // Disabling Tracking would normally cause an error to happen on all other types of requests.
    // In this case, it should trigger the callback with a long link.
    branchInstance->getAdvertiserInfo().disableTracking();

    LinkInfo linkInfo;
    initTestLink(linkInfo);

    linkInfo.createUrl(branchInstance, branchCallback);

    linkInfo.cancel();
}

TEST_F(LinkInfoTest, TestCreateLinkNoControlParams) {
    Branch *_branchInstance = BranchIO::Test::createTestInstance();
    LinkInfo linkInfo;
    std::string url = linkInfo.createLongUrl(_branchInstance);

    ASSERT_GT(url.size(), 0);

    linkInfo.cancel();
}
