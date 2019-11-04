#include <string>

// Use <gtest/gtest.h> when not using mocks.
// Otherwise <gmock/gmock.h> also brings in gtest.
#include <gtest/gtest.h>

#include <BranchIO/Util/Storage.h>

using namespace BranchIO;
using namespace std;

class StorageTest : public ::testing::Test
{
    virtual void SetUp() {
        Storage::instance().setDefaultScope(Storage::User);
        Storage::instance().clear(Storage::User);
    }

    virtual void TearDown() {
        Storage::instance().clear(Storage::User);
    }

 public:
    const std::string _testStringKey = "TestStringKey";
    const std::string _testStringValue = "TestValue";

    const std::string _testBooleanKey = "TestBooleanKey";
    const bool _testBooleanValue = true;
};

TEST_F(StorageTest, TestAddString)
{
    Storage::instance().setString(_testStringKey, _testStringValue);

    std::string result;
    bool found = Storage::instance().getString(_testStringKey, result);

    ASSERT_TRUE(found);
    ASSERT_EQ(_testStringValue, result);
}

TEST_F(StorageTest, TestAddBoolean)
{
    Storage::instance().setBoolean(_testBooleanKey, _testBooleanValue);

    bool result = false;
    bool found = Storage::instance().getBoolean(_testBooleanKey, result);

    ASSERT_TRUE(found);
    ASSERT_EQ(_testBooleanValue, result);
}

// Test loading values that are not there
TEST_F(StorageTest, TestNotFound) {
    std::string resultString;
    bool found = Storage::instance().getString(_testStringKey, resultString);
    ASSERT_FALSE(found);

    bool resultBool = true;
    found = Storage::instance().getBoolean(_testBooleanKey, resultBool);
    ASSERT_FALSE(found);
}

TEST_F(StorageTest, TestRemove) {
    StorageTest_TestAddString_Test();
    StorageTest_TestAddBoolean_Test();

    Storage::instance().remove(_testStringKey);
    Storage::instance().remove(_testBooleanKey);

    StorageTest_TestNotFound_Test();
}
