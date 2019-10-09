// Copyright (c) 2019 Branch Metrics, Inc.

#include "BranchIO/LinkInfo.h"

#include "BranchIO/JSONObject.h"

namespace BranchIO {

// JSON KEYS
const char *JSONKEY_TAGS = "tags";
const char *JSONKEY_ALIAS = "alias";
const char *JSONKEY_TYPE = "type";
const char *JSONKEY_DURATION = "duration";
const char *JSONKEY_CHANNEL = "channel";
const char *JSONKEY_FEATURE = "feature";
const char *JSONKEY_STAGE = "stage";
const char *JSONKEY_CAMPAIGN = "campaign";
const char *JSONKEY_DATA = "data";
const char *JSONKEY_URL = "url";


LinkInfo::LinkInfo() : PropertyManager() {
}

LinkInfo::LinkInfo(const JSONObject &jsonObject) : PropertyManager(jsonObject) {
}

LinkInfo::~LinkInfo() = default;

LinkInfo &
LinkInfo::addControlParameter(const std::string &key, const std::string &value) {
    return *this;
}

LinkInfo&
LinkInfo::addTag(const std::string &tag) {
    return *this;
}

LinkInfo&
LinkInfo::setAlias(const std::string &alias) {
    return doAddProperty(JSONKEY_ALIAS, alias);
}

LinkInfo &
LinkInfo::setCampaign(const std::string &campaign) {
    return doAddProperty(JSONKEY_CAMPAIGN, campaign);
}

LinkInfo &
LinkInfo::setChannel(const std::string &channel) {
    return doAddProperty(JSONKEY_CHANNEL, channel);
}

LinkInfo &
LinkInfo::setDuration(int duration) {
    return doAddProperty(JSONKEY_DURATION, duration);
}

LinkInfo &
LinkInfo::setFeature(const std::string &feature) {
    return doAddProperty(JSONKEY_FEATURE, feature);
}

LinkInfo &
LinkInfo::setStage(const std::string &stage) {
    return doAddProperty(JSONKEY_STAGE, stage);
}

LinkInfo &
LinkInfo::setType(int type) {
    return doAddProperty(JSONKEY_TYPE, type);
}


LinkInfo&
LinkInfo::doAddProperty(const char *name, const std::string &value) {
    addProperty(name, value);
    return *this;
}

LinkInfo&
LinkInfo::doAddProperty(const char *name, int value) {
    addProperty(name, value);
    return *this;
}

}  // namespace BranchIO
