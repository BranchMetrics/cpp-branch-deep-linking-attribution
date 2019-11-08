// Copyright (c) 2019 Branch Metrics, Inc.

#include "BranchIO/LinkInfo.h"
#include <Poco/URI.h>
#include <Poco/Base64Encoder.h>

#include "BranchIO/Defines.h"
#include "BranchIO/Util/StringUtils.h"

namespace BranchIO {

const char *BASE_LONG_URL = "https://bnc.lt/a/";

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

/**
 * (Internal) Fallback Callback.
 * This class allows for a failed short link to become a long link
 */
class LinkFallback : public IRequestCallback {
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
            _parentCallback(parent) {}

    virtual void onSuccess(int id, JSONObject jsonResponse) {
        // Call the original callback
        if (_parentCallback != NULL) {
            _parentCallback->onSuccess(id, jsonResponse);
        }
    }

    virtual void onError(int id, int error, std::string description) {
        // Attempt to create a Long Link
        std::string longUrl = _context.createLongUrl(_instance);

        JSONObject jsonObject;
        jsonObject.set(JSONKEY_URL, longUrl);

        if (_parentCallback != NULL) {
            _parentCallback->onSuccess(id, jsonObject);
        }
    }

    virtual void onStatus(int id, int error, std::string description) {
        // Call the original callback
        if (_parentCallback != NULL) {
            _parentCallback->onStatus(id, error, description);
        }
    }

 private:
    Branch *_instance;
    const LinkInfo &_context;
    IRequestCallback *_parentCallback;
};


LinkInfo::LinkInfo() : BaseEvent(Defines::APIEndpoint::URL, "LinkInfo") {
}

LinkInfo::~LinkInfo() = default;

LinkInfo &
LinkInfo::addControlParameter(const char *key, const std::string &value) {
    _controlParams.addProperty(key, value);
    addProperty(JSONKEY_DATA, _controlParams);
    return *this;
}

LinkInfo &
LinkInfo::addControlParameter(const char *key, int value) {
    _controlParams.addProperty(key, value);
    addProperty(JSONKEY_DATA, _controlParams);
    return *this;
}

LinkInfo&
LinkInfo::addControlParameter(const char *key, const PropertyManager &value) {
    _controlParams.addProperty(key, value);
    addProperty(JSONKEY_DATA, _controlParams);
    return *this;
}

LinkInfo&
LinkInfo::addTag(const std::string &tag) {
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

void
LinkInfo::createUrl(Branch *branchInstance, IRequestCallback *callback) {
    if (callback == NULL) {
        // Nothing to do as there is no way to provide a result.
        return;
    } else if (branchInstance == NULL || branchInstance->getBranchKey().size() == 0) {
        // The Branch Instance cannot be null
        callback->onError(0, 0, "Invalid Branch Instance");
        return;
    }

    branchInstance->sendEvent(*this, new LinkFallback(branchInstance, *this, callback));
}


std::string
LinkInfo::createLongUrl(Branch *branchInstance, const std::string &baseUrl) const {
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
    std::string params = _controlParams.toString();
    if (!_controlParams.isEmpty()) {
        std::stringstream ss;
        Poco::Base64Encoder b64enc(ss);
        b64enc.rdbuf()->setLineLength(0);
        b64enc << params << std::endl;

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
