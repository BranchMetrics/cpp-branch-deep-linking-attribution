#include <string>

// Use <gtest/gtest.h> when not using mocks.
// Otherwise <gmock/gmock.h> also brings in gtest.
#include <gtest/gtest.h>

#include <BranchIO/PropertyManager.h>

#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Foundation.Collections.h>

using namespace winrt::Windows::Data::Json;
using namespace winrt::Windows::Foundation::Collections;

using namespace BranchIO;
using namespace std;

class PropertyManagerTest : public ::testing::Test
{
};

TEST_F(PropertyManagerTest, TestSetters)
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
    
    ASSERT_EQ(strValue, jsonObject.getNamedString("keyStr"));
    winrt::Windows::Data::Json::JsonObject jObject = jsonObject.getWinRTJsonObj();
    ASSERT_EQ(intValue, jObject.GetNamedNumber(L"keyInt"));
    ASSERT_EQ(doubleValue, jObject.GetNamedNumber(L"keyDouble"));
    ASSERT_EQ(boolValue, jObject.GetNamedNumber(L"keyBool"));
}

TEST_F(PropertyManagerTest, TestGetStringProperty)
{
    PropertyManager mgr;
    mgr.addProperty("Foo", "Bar");

    std::string str = mgr.getStringProperty("Foo");
    ASSERT_EQ("Bar", str);
}

TEST_F(PropertyManagerTest, TestWriteSubProperty)
{
    PropertyManager mgr;
    mgr.addProperty("Foo", "Bar");

    PropertyManager subObject;
    subObject.addProperty("MyKey1", "MyValue1");

    mgr.addProperty("SubObject", subObject);

    cout << "TestWriteSubProperty:\t" << mgr.toString() << endl;
}
