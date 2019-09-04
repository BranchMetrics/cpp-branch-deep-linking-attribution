#include <string>

// Use <gtest/gtest.h> when not using mocks.
// Otherwise <gmock/gmock.h> also brings in gtest.
#include <gtest/gtest.h>

#include <BranchIO/PackagingInfo.h>
#include <BranchIO/Event/StandardEvent.h>

using namespace BranchIO;
using namespace std;
using BranchIO::Event::StandardEvent;

class AdvertiserInfoTest : public ::testing::Test
{
};

TEST_F(AdvertiserInfoTest, TestAddId)
{
    AdvertiserInfo info;

    info.addId(AdvertiserInfo::AdIdType::IDFA, "id_idfa");
    info.addId(AdvertiserInfo::AdIdType::GOOGLE_ADVERTISING_ID, "id_google");
    info.addId(AdvertiserInfo::AdIdType::WINDOWS_ADVERTISING_ID, "id_windows");
    info.addId(AdvertiserInfo::AdIdType::ROKU_RIDA, "id_roku");
    info.addId(AdvertiserInfo::AdIdType::SAMSUNG_IFA, "id_samsung");
    info.addId(AdvertiserInfo::AdIdType::LG_IFA, "id_lg");
    info.addId(AdvertiserInfo::AdIdType::PANASONIC_IFA, "id_panasonic");
    info.addId(AdvertiserInfo::AdIdType::PLAYSTATION_IFA, "id_playstation");
    info.addId(AdvertiserInfo::AdIdType::XBOX_MSAI, "id_xbox");

    JSONObject jsonObject = info.toJSON();
    ASSERT_EQ("id_idfa", jsonObject.get("idfa"));
    ASSERT_EQ("id_google", jsonObject.get("google_advertising_id"));
    ASSERT_EQ("id_windows", jsonObject.get("windows_advertising_id"));
    ASSERT_EQ("id_roku", jsonObject.get("roku_rida"));
    ASSERT_EQ("id_samsung", jsonObject.get("samsung_ifa"));
    ASSERT_EQ("id_lg", jsonObject.get("lg_ifa"));
    ASSERT_EQ("id_panasonic", jsonObject.get("panasonic_ifa"));
    ASSERT_EQ("id_playstation", jsonObject.get("playstation_ifa"));
    ASSERT_EQ("id_xbox", jsonObject.get("xbox_msai"));

    info.clear();
    jsonObject = info.toJSON();

    ASSERT_EQ(0, jsonObject.size());
}

TEST_F(AdvertiserInfoTest, TestPackaging)
{
    AppInfo appInfo;
    PackagingInfo packagingInfo("key_live_xxx");

    AdvertiserInfo &info = packagingInfo.getAdvertiserInfo();
    info.addId(AdvertiserInfo::AdIdType::WINDOWS_ADVERTISING_ID, "id_windows");
    info.addId(AdvertiserInfo::AdIdType::XBOX_MSAI, "id_xbox");

    StandardEvent event(StandardEvent::Type::PURCHASE);

    JSONObject jsonObject;
    event.package(packagingInfo, jsonObject);

    cout << "TestPackaging: " << jsonObject.stringify() << endl;

    JSONObject userData = jsonObject.extract("user_data");
    JSONObject advertising_ids = userData.extract("advertising_ids");
    ASSERT_EQ("id_windows", advertising_ids.get("windows_advertising_id"));
}

TEST_F(AdvertiserInfoTest, TestLAT_true)
{
    AppInfo appInfo;
    PackagingInfo packagingInfo("key_live_xxx");

    AdvertiserInfo &info = packagingInfo.getAdvertiserInfo();
    info.addId(AdvertiserInfo::AdIdType::WINDOWS_ADVERTISING_ID, "id_windows");
    info.addId(AdvertiserInfo::AdIdType::XBOX_MSAI, "id_xbox");
    info.limitAdTracking(true);

    StandardEvent event(StandardEvent::Type::PURCHASE);

    JSONObject jsonObject;
    event.package(packagingInfo, jsonObject);

    cout << "TestLAT_true: " << jsonObject.stringify() << endl;

    JSONObject userData = jsonObject.extract("user_data");
    ASSERT_FALSE(userData.has("advertising_ids"));
}
