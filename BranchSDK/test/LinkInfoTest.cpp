#include <string>

// Use <gtest/gtest.h> when not using mocks.
// Otherwise <gmock/gmock.h> also brings in gtest.
#include <gtest/gtest.h>

#include <BranchIO/LinkInfo.h>

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

    // cout << "***TestStringSetters***" << jsonObject->stringify() << endl;
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

TEST_F(LinkInfoTest, TestTags) {
    LinkInfo info;

    //void putTags(std::set<std::string> tags);

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

TEST_F(LinkInfoTest, TestJSONParams) {
    LinkInfo info;

    //void putParams(JSONObject params);

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
