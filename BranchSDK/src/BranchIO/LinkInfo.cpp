// Copyright (c) 2019 Branch Metrics, Inc.

#include "BranchIO/LinkInfo.h"
#include <Poco/URI.h>
#include <Poco/Base64Encoder.h>
#include <Poco/Util/Timer.h>

#include "BranchIO/Defines.h"
#include "BranchIO/Util/Log.h"
#include "BranchIO/Util/StringUtils.h"

using Poco::Mutex;

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

/**
 * (Internal) Fallback Callback.
 * This class allows for a failed short link to become a long link
 */
class LinkFallback : public IRequestCallback, Poco::Util::TimerTask {
 public:
    /**
     * Constructor.
     * @param instance Branch Instance
     * @param context LinkInfo context
     * @param parent Parent callback to be called after this has processed the response.
     */
    LinkFallback(Branch *instance, const LinkInfo &context, IRequestCallback *parent) :
            _instance(instance),
            _context(context),
            _parentCallback(parent) {
        // Schedule a timer.
        _timer.schedule(this, 2000, 10000);
    }

    void onSuccess(int id, JSONObject jsonResponse) {
        _timer.cancel(false);

        // Call the original callback
        if (_parentCallback != NULL) {
            _parentCallback->onSuccess(id, jsonResponse);
        }
    }

    void onError(int id, int error, std::string description) {
        // Attempt to create a Long Link
        BRANCH_LOG_D("Fallback and create a long link");

        std::string longUrl = _context.createLongUrl(_instance);

        if (longUrl.empty()) {
            // This is an actual failure.
            if (_parentCallback) {
                _parentCallback->onError(id, error, description);
            }
        } else {
            JSONObject jsonObject;
            jsonObject.set(JSONKEY_URL, longUrl);

            onSuccess(id, jsonObject);
        }
    }

    void onStatus(int id, int error, std::string description) {
        // Call the original callback
        if (_parentCallback != NULL) {
            _parentCallback->onStatus(id, error, description);
        }
    }

    /**
     * Timer Task.
     * If the timer fires, it will simply generate a long link and ignore the network result.
     */
    void run() {
        // Timer Task.
        onError(0, 0, "Link Fallback");
    }

 private:
    Poco::Util::Timer _timer;
    Branch *_instance;
    LinkInfo _context;
    IRequestCallback *_parentCallback;
};

LinkInfo::LinkInfo()
    : BaseEvent(Defines::APIEndpoint::URL, "LinkInfo") {
}

LinkInfo::LinkInfo(const LinkInfo& other) :
    BaseEvent(other),
    _controlParams(other._controlParams),
    _tagParams(other._tagParams) {
}

LinkInfo::~LinkInfo() = default;

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

    branchInstance->sendEvent(*this, new LinkFallback(branchInstance, *this, callback));
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

}  // namespace BranchIO
