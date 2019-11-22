#include <BranchIO/Event/StandardEvent.h>
#include <BranchIO/Request.h>

// Use <gtest/gtest.h> when not using mocks.
// Otherwise <gmock/gmock.h> also brings in gtest.
#include <gtest/gtest.h>
#include <BranchIO/PackagingInfo.h>

#include "Util.h"

using namespace std;
using namespace BranchIO;
using namespace testing;

class EventTest : public ::testing::Test
{
};

TEST_F(EventTest, TestCreateEvent)
{
    PackagingInfo packagingInfo(BranchIO::Test::getTestKey());
    StandardEvent event(StandardEvent::Type::PURCHASE);

    event.setAdType(Event::AdType::NATIVE);
    event.setAffiliation("My Affiliation");
    event.setCoupon("My Coupon");
    event.setCurrency("My CurrencyType");
    event.setCustomerEventAlias("My Alias");
    event.setDescription("My Description");
    event.setRevenue(99.99);
    event.setSearchQuery("My SearchQuery");
    event.setShipping(9.99);
    event.setTax(0.99);
    event.setTransactionId("My TransactionId");
    event.addCustomDataProperty("Custom", "My Custom Data Property");

    JSONObject jsonObject;
    event.package(packagingInfo, jsonObject);

    // cout << "TestCreateEvent: " << jsonObject.stringify() << endl;
}
