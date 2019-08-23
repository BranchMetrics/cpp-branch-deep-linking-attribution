#include <gtest/gtest.h>

#include <BranchIO/JSONObject.h>

using namespace std;
using namespace BranchIO;

class JSONObjectTest : public ::testing::Test
{

};

TEST_F(JSONObjectTest, NewStringify)
{
    JSONObject object;
    ASSERT_EQ(object.stringify(), "{}");
}

TEST_F(JSONObjectTest, OriginalStringify)
{
    JSONObject object;
    ostringstream oss;
    // Mainly want to make sure this still compiles.
    object.stringify(oss);
    ASSERT_EQ(oss.str(), "{}");
}

TEST_F(JSONObjectTest, PtrTest)
{
    JSONObject::Ptr ptr(new JSONObject);
    JSONObject object;
    object.set("foo", ptr);
    ASSERT_NO_THROW(object.stringify());
}
