// Copyright (c) 2019 Branch Metrics, Inc.

#include "BranchIO/LinkInfo.h"
#include <Poco/URI.h>
#include <Poco/Base64Encoder.h>
#include <Poco/Util/Timer.h>

#include "BranchIO/Util/APIClientSession.h"
#include "BranchIO/Branch.h"
#include "BranchIO/Defines.h"
#include "BranchIO/Util/Log.h"
#include "BranchIO/Util/StringUtils.h"

using Poco::Mutex;
using namespace Poco::Util;
using namespace std;

namespace BranchIO {

const char* const BASE_LONG_URL = "https://bnc.lt/a/";

// JSON KEYS
const char* const JSONKEY_TAGS = "tags";
const char* const JSONKEY_ALIAS = "alias";
const char* const JSONKEY_TYPE = "type";
const char* const JSONKEY_DURATION = "duration";
const char* const JSONKEY_CHANNEL = "channel";
const char* const JSONKEY_FEATURE = "feature";
const char* const JSONKEY_STAGE = "stage";
const char* const JSONKEY_CAMPAIGN = "campaign";
const char* const JSONKEY_DATA = "data";
const char* const JSONKEY_URL = "url";

LinkInfo::LinkInfo()
    : BaseEvent(Defines::APIEndpoint::URL, "LinkInfo"),
    _complete(true),
    _callback(nullptr),
    _branch(nullptr),
    _clientSession(nullptr) {
}

LinkInfo::~LinkInfo() {
    waitTillComplete();
}

bool
LinkInfo::isComplete() const {
    Mutex::ScopedLock _l(_mutex);
    return _complete;
}

void
LinkInfo::waitTillComplete() const {
    Mutex::ScopedLock _l(_mutex);
    while (!_complete) {
        _completeCondition.wait(_mutex);
    }
}

void
LinkInfo::cancel() {
    Mutex::ScopedLock _l(_mutex);
    _complete = true;
    _completeCondition.broadcast();

    if (!_clientSession) return;
    _clientSession->stop();
}

void
LinkInfo::setClientSession(IClientSession* clientSession) {
    Mutex::ScopedLock _l(_mutex);
    _clientSession = clientSession;
}

IClientSession*
LinkInfo::getClientSession() const {
    Mutex::ScopedLock _l(_mutex);
    return _clientSession;
}

IRequestCallback*
LinkInfo::getCallback() const {
    Mutex::ScopedLock _l(_mutex);
    return _callback;
}

LinkInfo &
LinkInfo::addControlParameter(const char *key, const std::string &value) {
    Mutex::ScopedLock _l(_mutex);
    _controlParams.addProperty(key, value);
    addProperty(JSONKEY_DATA, _controlParams);
    return *this;
}

LinkInfo &
LinkInfo::addControlParameter(const char *key, int value) {
    Mutex::ScopedLock _l(_mutex);
    _controlParams.addProperty(key, value);
    addProperty(JSONKEY_DATA, _controlParams);
    return *this;
}

LinkInfo&
LinkInfo::addControlParameter(const char *key, const PropertyManager &value) {
    Mutex::ScopedLock _l(_mutex);
    _controlParams.addProperty(key, value);
    addProperty(JSONKEY_DATA, _controlParams);
    return *this;
}

LinkInfo&
LinkInfo::addTag(const std::string &tag) {
    Mutex::ScopedLock _l(_mutex);
    _tagParams.add(tag);
    addProperty(JSONKEY_TAGS, _tagParams);
    return *this;
}

LinkInfo&
LinkInfo::setAlias(const std::string &alias) {
    return doAddProperty(JSONKEY_ALIAS, alias);
}

std::string
LinkInfo::getAlias() const {
    return PropertyManager::getStringProperty(JSONKEY_ALIAS);
}

LinkInfo &
LinkInfo::setCampaign(const std::string &campaign) {
    return doAddProperty(JSONKEY_CAMPAIGN, campaign);
}

std::string
LinkInfo::getCampaign() const {
    return PropertyManager::getStringProperty(JSONKEY_CAMPAIGN);
}

LinkInfo &
LinkInfo::setChannel(const std::string &channel) {
    return doAddProperty(JSONKEY_CHANNEL, channel);
}

std::string
LinkInfo::getChannel() const {
    return PropertyManager::getStringProperty(JSONKEY_CHANNEL);
}

LinkInfo &
LinkInfo::setDuration(int duration) {
    return doAddProperty(JSONKEY_DURATION, duration);
}

LinkInfo &
LinkInfo::setFeature(const std::string &feature) {
    return doAddProperty(JSONKEY_FEATURE, feature);
}

std::string
LinkInfo::getFeature() const {
    return PropertyManager::getStringProperty(JSONKEY_FEATURE);
}

LinkInfo &
LinkInfo::setStage(const std::string &stage) {
    return doAddProperty(JSONKEY_STAGE, stage);
}

std::string
LinkInfo::getStage() const {
    return PropertyManager::getStringProperty(JSONKEY_STAGE);
}

LinkInfo &
LinkInfo::setType(int type) {
    return doAddProperty(JSONKEY_TYPE, type);
}

// @todo(jdee): Make these args references. There's no need for all these pointers and null checks.
void
LinkInfo::createUrl(Branch *branchInstance, IRequestCallback *callback) {
    if (callback == NULL) {
        // Nothing to do as there is no way to provide a result.
        return;
    } else if (branchInstance == NULL || branchInstance->getBranchKey().empty()) {
        // The Branch Instance cannot be null
        callback->onError(0, 0, "Invalid Branch Instance");
        return;
    }

    Mutex::ScopedLock _l(_mutex);
    _complete = false;
    /*
     * Start a thread to make the request. Executes LinkInfo::run().
     */
    _thread.start(*this);
}

std::string
LinkInfo::createLongUrl(Branch *branchInstance, const std::string &baseUrl) const {
    Mutex::ScopedLock _l(_mutex);

    // TODO(andyp): Handle response from setIdentity if there is a base URL in the response

    std::string longUrl;
    longUrl += (baseUrl.size() > 0 ? baseUrl : BASE_LONG_URL);
    longUrl += branchInstance->getBranchKey();

    Poco::URI uri(longUrl);

    for (JSONArray::ConstIterator it = _tagParams.begin(); it != _tagParams.end(); ++it) {
        uri.addQueryParameter(JSONKEY_TAGS, it->convert<std::string>());
    }

    std::string alias = getAlias();
    if (alias.size() > 0) {
        uri.addQueryParameter(JSONKEY_ALIAS, alias);
    }

    std::string channel = getChannel();
    if (channel.size() > 0) {
        uri.addQueryParameter(JSONKEY_CHANNEL, channel);
    }

    std::string feature = getFeature();
    if (feature.size() > 0) {
        uri.addQueryParameter(JSONKEY_FEATURE, feature);
    }

    std::string stage = getStage();
    if (stage.size() > 0) {
        uri.addQueryParameter(JSONKEY_STAGE, stage);
    }

    std::string campaign = getCampaign();
    if (campaign.size() > 0) {
        uri.addQueryParameter(JSONKEY_CAMPAIGN, campaign);
    }

    // Take the controlParams, convert those to a string, and base64 them
    // Note that Poco Base64 introduces by default line endings after 74?? characters.  Call setLineLength(0) to fix.
    // Note further that Poco Base64 implementation appears to require an eol, or it doesn't complete the encoding.
    if (!_controlParams.isEmpty()) {
        std::stringstream ss;
        Poco::Base64Encoder b64enc(ss);
        b64enc.rdbuf()->setLineLength(0);
        b64enc << _controlParams.toString() << std::endl;

        uri.addQueryParameter(JSONKEY_DATA, ss.str());
    }

    return uri.toString();
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

void
LinkInfo::onSuccess(int id, JSONObject response) {
    auto callback = getCallback();
    if (!callback) return;

    callback->onSuccess(id, response);
}

void
LinkInfo::onStatus(int id, int error, std::string description) {
    /*
     * onStatus is called for transient errors. Give up and create
     * a long link on the first error.
     */
    onError(id, error, description);
}

void
LinkInfo::onError(int id, int error, std::string description) {
    auto callback = getCallback();
    if (!callback) return;

    // Attempt to create a Long Link
    BRANCH_LOG_D("Fallback and create a long link");

    std::string longUrl = createLongUrl(_branch);

    if (longUrl.empty()) {
        // This is an actual failure.
        callback->onError(id, error, description);
    } else {
        JSONObject jsonObject;
        jsonObject.set(JSONKEY_URL, longUrl);

        callback->onSuccess(id, jsonObject);
    }
}

void
LinkInfo::run() {
    /**
     * Make /v1/url request synchronously with APIClientSession
     */

    // @todo(jdee): Untangle constant handling in this SDK.

    // 1. Build JSON payload using key & identity from Branch instance.
    JSONObject payload(*this);
    payload.set("branch_key", _branch->getBranchKey());
    string identity(_branch->getAppInfo().getDeveloperIdentity());
    if (!identity.empty()) {
        payload.set("identity", identity);
    }

    auto mockClientSession = getClientSession();
    // 2. POST & call back
    if (mockClientSession) {
        // Use in unit tests.
        mockClientSession->post("/v1/url", payload, *this);
    } else {
        APIClientSession clientSession(BRANCH_IO_URL_BASE);
        // synchronous call but calls callback. blocks till callback invoked.
        setClientSession(&clientSession);
        clientSession.post("/v1/url", payload, *this);
        setClientSession(nullptr);
    }

    // Thread exiting, unblock destructor.
    cancel();
}

}  // namespace BranchIO
