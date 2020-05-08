#include <string>

// Use <gtest/gtest.h> when not using mocks.
// Otherwise <gmock/gmock.h> also brings in gtest.
#include <gtest/gtest.h>

#include <BranchIO/DeviceInfo.h>

using namespace BranchIO;
using namespace std;

bool startsWith(std::string str, std::string start)
{
    return (str.rfind(start, 0) == 0);
}

TEST(BranchDeviceInfoTest, TestSetters)
{
    DeviceInfo info;

    info.setBrand("My brand")
        .setIPAddress("My IPAddress")
        .setMACAddress("My MACAddress")
        .setModel("My model")
        .setOs("My os")
        .setOsVersion("My osVersion")
        .setSDK("My sdk")
        .setSDKVersion("My sdkVersion")
        .setUserAgent("My userAgent");

    std::string str = info.toString();
    ASSERT_GT(str.length(), 0);

    JSONObject jsonObject = JSONObject::parse(str);

    for (JSONObject::ConstIterator it = jsonObject.begin(); it != jsonObject.end(); ++it) {
        std::string value = it->second;

        ASSERT_STREQ("My", value.substr(0, 2).c_str());
    }
}
