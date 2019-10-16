#include <BranchIO/Util/MacAddress.h>
#include <gtest/gtest.h>

using namespace BranchIO;
using namespace std;
using namespace testing;

TEST(MacAddressTest, DefaultInitialization) {
    ASSERT_TRUE(MacAddress().empty());
}

TEST(MacAddressTest, StringInitialization) {
    MacAddress address("aa:bb:cc:dd:ee:ff");
    ASSERT_EQ(address.toString(), "aa:bb:cc:dd:ee:ff");
}

TEST(MacAddressTest, VectorInitialization) {
    const unsigned char data[] = { 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };
    const size_t numEntries = sizeof(data)/sizeof(unsigned char);
    vector<unsigned char> v(data, data + numEntries);

    ASSERT_EQ(v.size(), numEntries);

    MacAddress address(v);
    MacAddress::ComponentList components(address.getComponents());
    ASSERT_EQ(components.size(), numEntries);
    ASSERT_EQ(components[0], "aa");
    ASSERT_EQ(components[1], "bb");
    ASSERT_EQ(components[2], "cc");
    ASSERT_EQ(components[3], "dd");
    ASSERT_EQ(components[4], "ee");
    ASSERT_EQ(components[5], "ff");

    ASSERT_TRUE(address.valid());
}

TEST(MacAddressTest, ColonComponentSplit) {
    MacAddress address("aa:bb:cc:dd:ee:ff");
    MacAddress::ComponentList components(address.getComponents());
    ASSERT_EQ(components.size(), 6);
    ASSERT_EQ(components[0], "aa");
    ASSERT_EQ(components[1], "bb");
    ASSERT_EQ(components[2], "cc");
    ASSERT_EQ(components[3], "dd");
    ASSERT_EQ(components[4], "ee");
    ASSERT_EQ(components[5], "ff");

    ASSERT_TRUE(address.valid());
}

TEST(MacAddressTest, HyphenComponentSplit) {
    MacAddress address("aa-bb-cc-dd-ee-ff");
    MacAddress::ComponentList components(address.getComponents());
    ASSERT_EQ(components.size(), 6);
    ASSERT_EQ(components[0], "aa");
    ASSERT_EQ(components[1], "bb");
    ASSERT_EQ(components[2], "cc");
    ASSERT_EQ(components[3], "dd");
    ASSERT_EQ(components[4], "ee");
    ASSERT_EQ(components[5], "ff");

    ASSERT_TRUE(address.valid());
}

TEST(MacAddressTest, Validation) {
    MacAddress address("foo");
    ASSERT_FALSE(address.valid());
}

TEST(MacAddressTest, Equality) {
    MacAddress mac1("aa:bb:cc:dd:ee:ff");
    MacAddress mac2("aa-bb-cc-dd-ee-ff");

    ASSERT_EQ(mac1, mac2);
}

TEST(MacAddressTest, InEquality) {
    MacAddress mac1("aa:bb:cc:dd:ee:ff");
    MacAddress mac2("0a-bb-cc-dd-ee-ff");

    ASSERT_NE(mac1, mac2);
}
