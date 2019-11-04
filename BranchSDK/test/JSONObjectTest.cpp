#include <gtest/gtest.h>

#include <BranchIO/JSONObject.h>
#include <BranchIO/JSONArray.h>
//#include <Poco/JSON/Array.h>

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

TEST_F(JSONObjectTest, TestJSONArray) {
    JSONArray jsonArray;

    jsonArray.add("One");
    jsonArray.add("Two");
    jsonArray.add("Three");

//    std::string str = jsonArray.stringify();
//    cout << "TestJSONArray: " << str << endl;

    ASSERT_EQ(jsonArray.size(), 3);
}

TEST_F(JSONObjectTest, TestJSONObjectAddArray) {
    JSONArray jsonArray;

    jsonArray.add("One");
    jsonArray.add("Two");
    jsonArray.add("Three");

    JSONObject object;
    object.set("foo", "bar");
    object.set("array", jsonArray);

    std::string str = object.stringify();
    cout << "TestJSONObjectAddArray: " << str << endl;
}

TEST_F(JSONObjectTest, TestJSONObjectNullSet) {
    JSONObject object;
    JSONObject::Ptr ptr = nullptr;
    object.set("foo", ptr);
    ASSERT_NO_THROW(object.stringify());
}