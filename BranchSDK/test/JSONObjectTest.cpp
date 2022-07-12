#include <gtest/gtest.h>

#include <BranchIO/JSONObject.h>

using namespace std;
using namespace BranchIO;

TEST(JSONObjectTest, EmptyTest) {
    JSONObject object;
    ASSERT_TRUE(object.isEmpty());
}

TEST(JSONObjectTest, NonEmptyTest) {
    JSONObject object;
    object.set("foo", "bar");
    ASSERT_FALSE(object.isEmpty());
}

TEST(JSONObjectTest, NewStringify)
{
    JSONObject object;
    ASSERT_EQ(object.stringify(), "{}");
}



// null cannot be converted to a JSONObject
TEST(JSONObjectTest, TestParseNull) {
}

TEST(JSONObjectTest, TestParseObject) {
    JSONObject object(JSONObject::parse("{\"foo\": \"bar\"}"));
    ASSERT_EQ(object.getNamedString("foo"), "bar");
}
