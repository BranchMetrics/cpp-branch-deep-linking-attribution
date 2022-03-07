// Copyright (c) 2019-21 Branch Metrics, Inc.

#include "BranchIO/LinkInfo.h"
#include "BranchIO/Util/APIClientSession.h"
#include "BranchIO/Branch.h"
#include "BranchIO/Defines.h"
#include "BranchIO/Util/Log.h"
#include "BranchIO/Util/StringUtils.h"
#include <winrt/Windows.Foundation.h>

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
    _canceled(false),
    _callback(nullptr),
    _branch(nullptr),
    _clientSession(nullptr) {
}

LinkInfo::~LinkInfo() {
    waitTillComplete();
}

bool
LinkInfo::isComplete() const {
    scoped_lock _l(_mutex);
    return _complete;
}

bool
LinkInfo::isCanceled() const {
    scoped_lock _l(_mutex);
    return _canceled;
}

void
LinkInfo::waitTillComplete() const {

    unique_lock<mutex> _l(_mutex);
    while (!_complete) {
        _completeCondition.wait(_l);
    }
}

void
LinkInfo::complete() {
    scoped_lock _l(_mutex);
    _complete = true;
    _completeCondition.notify_all();
}

void
LinkInfo::cancel() {
    scoped_lock _l(_mutex);
    _canceled = true;

    if (!_clientSession) return;
    _clientSession->stop();
}

void
LinkInfo::setClientSession(IClientSession* clientSession) {
    scoped_lock _l(_mutex);
    _clientSession = clientSession;
}

IClientSession*
LinkInfo::getClientSession() const {
    scoped_lock _l(_mutex);
    return _clientSession;
}

IRequestCallback*
LinkInfo::getCallback() const {
    scoped_lock _l(_mutex);
    return _callback;
}

Branch*
LinkInfo::getBranchInstance() const {
    scoped_lock _l(_mutex);
    return _branch;
}

LinkInfo &
LinkInfo::addControlParameter(const String& key, const String& value) {
    scoped_lock _l(_mutex);;
    _controlParams.addProperty(key.str().c_str(), value.str());
    addProperty(JSONKEY_DATA, _controlParams);
    return *this;
}

LinkInfo &
LinkInfo::addControlParameter(const String& key, int value) {
    scoped_lock _l(_mutex);
    _controlParams.addProperty(key.str().c_str(), value);
    addProperty(JSONKEY_DATA, _controlParams);
    return *this;
}

LinkInfo&
LinkInfo::addControlParameter(const String& key, const PropertyManager& value) {
    scoped_lock _l(_mutex);
    _controlParams.addProperty(key.str().c_str(), value);
    addProperty(JSONKEY_DATA, _controlParams);
    return *this;
}

LinkInfo&
LinkInfo::addTag(const String& tag) {
    scoped_lock _l(_mutex);
    _tagParams.push_back(tag.str());
    addProperty(JSONKEY_TAGS, _tagParams);
    return *this;
}

LinkInfo&
LinkInfo::setAlias(const String& alias) {
    return doAddProperty(JSONKEY_ALIAS, alias.str());
}

std::string
LinkInfo::getAlias() const {
    return PropertyManager::getStringProperty(JSONKEY_ALIAS);
}

LinkInfo &
LinkInfo::setCampaign(const String& campaign) {
    return doAddProperty(JSONKEY_CAMPAIGN, campaign.str());
}

std::string
LinkInfo::getCampaign() const {
    return PropertyManager::getStringProperty(JSONKEY_CAMPAIGN);
}

LinkInfo &
LinkInfo::setChannel(const String& channel) {
    return doAddProperty(JSONKEY_CHANNEL, channel.str());
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
LinkInfo::setFeature(const String& feature) {
    return doAddProperty(JSONKEY_FEATURE, feature.str());
}

std::string
LinkInfo::getFeature() const {
    return PropertyManager::getStringProperty(JSONKEY_FEATURE);
}

LinkInfo &
LinkInfo::setStage(const String& stage) {
    return doAddProperty(JSONKEY_STAGE, stage.str());
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

    {
        scoped_lock _l(_mutex);
        _complete = false;
        _branch = branchInstance;
        _callback = callback;
    }

    /*
     * Start a thread to make the request. Executes LinkInfo::run().
     */
    _thread = thread([this] { this->run(); });
}

std::string
LinkInfo::createLongUrl(Branch* branchInstance, const String& baseUrl) const {
    scoped_lock _l(_mutex);

    // TODO(andyp): Handle response from setIdentity if there is a base URL in the response

    string longUrl;
    string sBaseUrl(baseUrl.str());
    longUrl += (sBaseUrl.size() > 0 ? sBaseUrl : BASE_LONG_URL);
    longUrl += branchInstance->getBranchKey();

    string uri(longUrl);
    string query;

    for (std::vector<std::string>::const_iterator it = _tagParams.begin(); it != _tagParams.end(); ++it) {
        appendQueryParameters(query, JSONKEY_TAGS, it->c_str());
    }

    std::string alias = getAlias();
    if (alias.size() > 0) {
        appendQueryParameters(query, JSONKEY_ALIAS, alias);
    }

    std::string channel = getChannel();
    if (channel.size() > 0) {
        appendQueryParameters(query, JSONKEY_CHANNEL, channel);
    }

    std::string feature = getFeature();
    if (feature.size() > 0) {
        appendQueryParameters(query, JSONKEY_FEATURE, feature);
    }

    std::string stage = getStage();
    if (stage.size() > 0) {
        appendQueryParameters(query, JSONKEY_STAGE, stage);
    }

    std::string campaign = getCampaign();
    if (campaign.size() > 0) {
        appendQueryParameters(query, JSONKEY_CAMPAIGN, campaign);
    }

    if (!_controlParams.isEmpty()) {
        const std::string ss = StringUtils::EncodeBase64(reinterpret_cast<const BYTE*>(_controlParams.toString().data()), _controlParams.toString().size());
        appendQueryParameters(query, JSONKEY_DATA, ss);
    }

    return uri.append(query);
    }

void LinkInfo::appendQueryParameters(string& query, const char* tag, string value) const
{
    if (query.empty())
        query.append("?");
    else
        query.append("&");
    query.append(StringUtils::UriEncode(string(tag)) + "=" + StringUtils::UriEncode(value));
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

    auto longUrl = createLongUrl(_branch);

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
    /*
     * Ensure that complete() is called no matter how we exit this frame.
     */
    struct Completer {
        Completer(LinkInfo& object, void (LinkInfo::* method)()) : _object(object), _method(method) {}
        ~Completer() {
            // Thread exiting, unblock destructor.
            BRANCH_LOG_D("Terminating thread for /v1/url POST");
            (_object.*_method)();
        }
        LinkInfo& _object;
        void (LinkInfo::* _method)();
    } completer(*this, &LinkInfo::complete);

    try {
        BRANCH_LOG_D("Starting thread for /v1/url POST");
        /**
         * Make /v1/url request synchronously with APIClientSession
         */

        // @todo(jdee): Untangle constant handling in this SDK.

        // 1. Build JSON payload using key & identity from Branch instance.
        JSONObject payload(*this);
        auto branch = getBranchInstance();
        payload.set("branch_key", branch->getBranchKey());

        auto clientSession = getClientSession();
        // 2. POST & call back
        JSONObject result;
        if (clientSession) {
            // Use in unit tests.
            clientSession->post("/v1/url", payload, *this, result);
        } else {
            APIClientSession apiClientSession(BRANCH_IO_URL_BASE);

            // Check before a blocking socket operation
            if (isCanceled()) return;

            // synchronous call but calls callback. blocks till callback invoked.
            setClientSession(&apiClientSession);
            // blocking socket write & read
            apiClientSession.post("/v1/url", payload, *this, result);
            setClientSession(nullptr);
        }
    }
    catch (winrt::hresult_error const& ex) {
        // WinRT Exceptions
        BRANCH_LOG_E("Exception in LinkInfo thread [" << ex.code() << "]: " << ex.message().c_str());
    }
    catch (std::exception& e) {
        // Other STL exceptions
        BRANCH_LOG_E("Exception in LinkInfo thread: " << e.what());
    }
    catch (...) {
        // Anything else
        BRANCH_LOG_E("Unexpected exception in LinkInfo thread.");
    }
}

}  // namespace BranchIO
