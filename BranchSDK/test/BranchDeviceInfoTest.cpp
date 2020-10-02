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
	// Note os_version is stripped out because it is converted to a blank string.
	// This is why this test case passes.
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

TEST(BranchDeviceInfoTest, TestOsVersion)
{
    DeviceInfo info;

    info.setOsVersion("6.2 (Build 9200)");

    string osVersion = info.getStringProperty("os_version");

    ASSERT_EQ(osVersion, "6.2");
}
