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

    bool found = Storage::instance().has(_testStringKey);
    ASSERT_TRUE(found);

    std::string result = Storage::instance().getString(_testStringKey);
    ASSERT_EQ(_testStringValue, result);
}

TEST_F(StorageTest, TestAddBoolean)
{
    Storage::instance().setBoolean(_testBooleanKey, _testBooleanValue);

    bool found = Storage::instance().has(_testBooleanKey);
    ASSERT_TRUE(found);

    bool result = Storage::instance().getBoolean(_testBooleanKey);
    ASSERT_EQ(_testBooleanValue, result);
}

TEST_F(StorageTest, TestDefaultBoolean)
{
    bool result = Storage::instance().getBoolean(_testBooleanKey);
    ASSERT_FALSE(result);

    result = Storage::instance().getBoolean(_testBooleanKey, true);
    ASSERT_TRUE(result);
}

TEST_F(StorageTest, TestDefaultString)
{
    std::string result = Storage::instance().getString(_testStringKey);
    ASSERT_TRUE(result.empty());

    result = Storage::instance().getString(_testStringKey, _testStringValue);
    ASSERT_EQ(_testStringValue, result);
}

// Test loading values that are not there
TEST_F(StorageTest, TestKeyNotFound) {
    bool found = Storage::instance().has(_testStringKey);
    ASSERT_FALSE(found);

    found = Storage::instance().has(_testBooleanKey);
    ASSERT_FALSE(found);
}

TEST_F(StorageTest, TestAddRemove) {
    StorageTest_TestAddString_Test();
    StorageTest_TestAddBoolean_Test();

    Storage::instance().remove(_testStringKey);
    Storage::instance().remove(_testBooleanKey);

    StorageTest_TestKeyNotFound_Test();
}

TEST_F(StorageTest, TestDefaultScope) {
    Storage::instance().setDefaultScope(Storage::Default);
    StorageTest_TestAddRemove_Test();
}

TEST_F(StorageTest, TestHostScope) {
    Storage::instance().setDefaultScope(Storage::Host);
    StorageTest_TestAddRemove_Test();
}

TEST_F(StorageTest, TestUserScope) {
    Storage::instance().setDefaultScope(Storage::User);
    StorageTest_TestAddRemove_Test();
}
