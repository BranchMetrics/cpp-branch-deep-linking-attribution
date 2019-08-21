#include <string>

// Use <gtest/gtest.h> when not using mocks.
// Otherwise <gmock/gmock.h> also brings in gtest.
#include <gtest/gtest.h>

#include <BranchIO/PropertyManager.h>

using namespace BranchIO;
using namespace std;

class BranchPropertyManagerTest : public ::testing::Test
{
};

TEST_F(BranchPropertyManagerTest, TestSetters)
{
    PropertyManager mgr;

    std::string strValue = "value1";
    int intValue = 10;
    double doubleValue = 12.34;
    bool boolValue = true;

    mgr.addProperty("keyStr", strValue);
    mgr.addProperty("keyInt", intValue);
    mgr.addProperty("keyDouble", doubleValue);
    mgr.addProperty("keyBool", boolValue);

    JSONObject jsonObject = mgr.toJSON();

    ASSERT_EQ(strValue, jsonObject.get("keyStr"));
    ASSERT_EQ(intValue, jsonObject.get("keyInt"));
    ASSERT_EQ(doubleValue, jsonObject.get("keyDouble"));
    ASSERT_EQ(boolValue, jsonObject.get("keyBool"));
}
