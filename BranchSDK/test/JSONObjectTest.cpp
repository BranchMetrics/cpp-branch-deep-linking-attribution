#include <gtest/gtest.h>

#include <Poco/JSON/JSONException.h>
#include <BranchIO/JSONObject.h>
#include <BranchIO/JSONArray.h>
//#include <Poco/JSON/Array.h>

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

TEST(JSONObjectTest, OriginalStringify)
{
    JSONObject object;
    ostringstream oss;
    // Mainly want to make sure this still compiles.
    object.stringify(oss);
    ASSERT_EQ(oss.str(), "{}");
}

TEST(JSONObjectTest, PtrTest)
{
    JSONObject::Ptr ptr(new JSONObject);
    JSONObject object;
    object.set("foo", ptr);
    ASSERT_NO_THROW(object.stringify());
}

TEST(JSONObjectTest, TestJSONArray) {
    JSONArray jsonArray;

    jsonArray.add("One");
    jsonArray.add("Two");
    jsonArray.add("Three");

    ASSERT_EQ(jsonArray.size(), 3);
}

TEST(JSONObjectTest, TestJSONObjectAddArray) {
    JSONArray jsonArray;

    jsonArray.add("One");
    jsonArray.add("Two");
    jsonArray.add("Three");

    JSONObject object;
    object.set("foo", "bar");
    object.set("array", jsonArray);

    std::string str = object.stringify();
}

TEST(JSONObjectTest, TestJSONObjectNullSet) {
    JSONObject object;
    JSONObject::Ptr ptr = nullptr;
    object.set("foo", ptr);
    ASSERT_NO_THROW(object.stringify());
}

TEST(JSONObjectTest, TestParseEmpty) {
    ASSERT_THROW(JSONObject::parse(""), Poco::JSON::JSONException);
}

// null cannot be converted to a JSONObject
TEST(JSONObjectTest, TestParseNull) {
    ASSERT_THROW(JSONObject::parse("null"), Poco::JSON::JSONException);
}

// an array cannot be converted to a JSONObject
TEST(JSONObjectTest, TestParseArray) {
    ASSERT_THROW(JSONObject::parse("[]"), Poco::JSON::JSONException);
}

TEST(JSONObjectTest, TestParseObject) {
    JSONObject object(JSONObject::parse("{\"foo\": \"bar\"}"));
    ASSERT_EQ(object.getValue<string>("foo"), "bar");
}
