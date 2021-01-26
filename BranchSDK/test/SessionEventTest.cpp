#include <BranchIO/Defines.h>
#include <BranchIO/Event/SessionEvent.h>
#include <gtest/gtest.h>

using namespace testing;
using namespace std;
using namespace BranchIO;

TEST(SessionOpenEventTest, TestLinkIdentifier)
{
    SessionOpenEvent openEvent;
    openEvent.setLinkUrl("test://open?link_click_id=811645508874040815");
    auto linkIdentifier = openEvent.getStringProperty(Defines::JSONKEY_APP_LINK_URL);
    ASSERT_EQ(linkIdentifier, "811645508874040815");
}
