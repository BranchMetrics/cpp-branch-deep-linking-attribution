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

TEST(LinkInfoTest, TestStringSetters) {
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
}

TEST(LinkInfoTest, TestIntegerSetters) {
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
}

TEST(LinkInfoTest, TestControlParams) {
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
}

TEST(LinkInfoTest, TestTagParams) {
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
}
