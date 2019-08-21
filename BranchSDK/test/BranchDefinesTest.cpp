#include <string>

// Use <gtest/gtest.h> when not using mocks.
// Otherwise <gmock/gmock.h> also brings in gtest.
#include <gtest/gtest.h>

#include <BranchIO/Defines.h>

using namespace BranchIO;
using namespace std;

class BranchDefinesTest : public ::testing::Test
{
};

TEST_F(BranchDefinesTest, NoBranchValue)
{
    // TODO: Why NO_BRANCH_VALUE, not just NO_VALUE?
    // For that matter, given that everything is in a branch_io namespace,
    // there's no point to adding Branch to everything. That's like Obj-C
    // with no namespacing.
    ASSERT_EQ(string(), Defines::NO_BRANCH_VALUE);
}
