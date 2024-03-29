// Copyright (c) 2019-21 Branch Metrics, Inc.

#ifndef BRANCHIO_LINKINFO_H__
#define BRANCHIO_LINKINFO_H__

#include <string>
#include "BranchIO/Event/BaseEvent.h"
#include "BranchIO/IRequestCallback.h"
#include "BranchIO/fwd.h"
#include "BranchIO/String.h"
#include <mutex>

namespace BranchIO {

/**
 * Link Information.
 */
class BRANCHIO_DLL_EXPORT LinkInfo :
    protected BaseEvent,
    protected virtual IRequestCallback {
 public:
    /**
     * An Integer value indicating the calculation type of the referral code. In this case,
     * the referral code can be applied continually.
     */
    static const int LINK_TYPE_UNLIMITED_USE = 0;

    /**
     * An Integer value indicating the link type. In this case, the link can be used an
     * unlimited number of times.
     */
    static const int LINK_TYPE_ONE_TIME_USE = 1;

 public:
    /**
     * Constructor.
     */
    LinkInfo();

    /**
     * Destructor. Blocks until request is complete or canceled.
     */
    ~LinkInfo();

    /**
     * Any other params to be added; you can define your own.
     *
     * @param key   A string with the key for the parameter
     * @param value A string with the value for the parameter
     * @return This object for chaining builder methods
     */
    LinkInfo& addControlParameter(const String& key, const String &value);

    /**
     * Any other params to be added; you can define your own.
     *
     * @param key   A string with the key for the parameter
     * @param value An integer with the value for the parameter
     * @return This object for chaining builder methods
     */
    LinkInfo& addControlParameter(const String& key, int value);

    /**
     * Any other params to be added; you can define your own.
     *
     * @param key   A string with the key for the parameter
     * @param value A PropertyManager with the value for the parameter
     * @return This object for chaining builder methods
     */
    LinkInfo& addControlParameter(const String& key, const PropertyManager& value);

    /**
     * Adds a tag to the collection associated with a deep link.
     *
     * @param tag A tag associated with a deep link.
     * @return This object for chaining builder methods
     */
    LinkInfo& addTag(const String& tag);

    /**
     * Sets the alias for this link.
     *
     * @param alias A string value containing the desired alias name to add.
     * @return This object for chaining builder methods
     */
    LinkInfo& setAlias(const String& alias);

    /**
     * A string value that represents the campaign associated with this link.
     *
     * @param campaign A string value specifying the campaign.
     * @return This object for chaining builder methods
     */
    LinkInfo& setCampaign(const String& campaign);

    /**
     * [Optional] The channel in which the link will be shared. eg: "facebook",
     * "text_message".
     *
     * @param channel A string value containing the channel which the link
     *                belongs to. (max 128 characters).
     * @return This object for chaining builder methods
     */
    LinkInfo& setChannel(const String& channel);

    /**
     * [Optional] You can set the duration manually. This is the time that
     * Branch allows a click to remain outstanding and be eligible to be matched
     * with a new app session.
     *
     * @param duration An integer value in seconds.
     * @return This object for chaining builder methods
     */
    LinkInfo& setDuration(int duration);

    /**
     * [Optional] The feature in which the link will be used. eg: "invite",
     * "referral", "share", "gift", etc.
     *
     * @param feature A string specifying the feature. (max 128 characters).
     * @return This object for chaining builder methods
     */
    LinkInfo& setFeature(const String &feature);

    /**
     * A string value that represents the stage of the user in the app. eg:
     * "level1", "logged_in", etc.
     *
     * @param stage A string value specifying the stage.
     * @return This object for chaining builder methods
     */
    LinkInfo& setStage(const String &stage);

    /**
     * Adds a type to the link.
     *
     * @param type An Integer value of the type specified. Valid values are:
     *             <ul>
     *             <li>{@link BranchIO::LinkInfo#LINK_TYPE_UNLIMITED_USE}</li>
     *             <li>{@link BranchIO::LinkInfo#LINK_TYPE_ONE_TIME_USE}</li>
     *             </ul>
     * @return This object for chaining builder methods
     */
    LinkInfo& setType(int type);

    /**
     * Create a Branch Url with the given deep link parameters and link properties.
     * Note that this will "fall back" to generating a Long URL on error.
     * Note that if the callback is null, no request will be made.
     * @param branchInstance Branch Instance
     * @param callback Callback to fire with success or failure notification.
     */
    void createUrl(Branch *branchInstance, IRequestCallback *callback);

    /**
     * Create a long Url with the given deep link parameters and link properties.
     * Note that this does not require an active network connection.
     * @param branchInstance Branch Instance.
     * @param baseUrl Non-Default base to use for forming the url
     * @return A url with the given deep link parameters.
     */
    std::string createLongUrl(Branch* branchInstance, const String& baseUrl = String()) const;

    using PropertyManager::toString;

    /**
     * Determine if the request has received a response or been canceled.
     * @return true if complete, false otherwise
     */
    bool isComplete() const;

    /**
     * Block execution until the request is complete.
     */
    void waitTillComplete() const;

    /**
     * Cancel the request. Prevents further callbacks. Allows
     * immediate destruction of the object.
     */
    void cancel();

    /**
     * Indicates whether cancel() has been called.
     * @return true if cancel previously called, false otherwise
     */
    bool isCanceled() const;

    /**
     * For testing. Can use a mock session here.
     * @param clientSession An IClientSession instance to use.
     */
    void setClientSession(IClientSession* clientSession);

 protected:
    /**
     * @copydoc IRequestCallback::onSuccess
     */
    void onSuccess(int id, JSONObject jsonResponse);

    /**
     * @copydoc IRequestCallback::onError
     */
    void onError(int id, int error, std::string description);

    /**
     * @copydoc IRequestCallback::onStatus
     */
    void onStatus(int id, int error, std::string description);

    /**
     * Execute the URL request. 
     */
    void run();

    /**
     * Get the current client session.
     * @return an IClientSession pointer or nullptr
     */
    IClientSession* getClientSession() const;

    /**
     * Get the current callback, if any.
     * @return an IRequestCallback pointer or nullptr
     */
    IRequestCallback* getCallback() const;

    /**
     * Get the Branch instance passed to createUrl.
     * @return a Branch pointer or nullptr
     */
    Branch* getBranchInstance() const;

 private:
    /**
     * Add a string value property to the set.
     * Note that if the value is empty, this effectively removes the key.
     * @param name Key name
     * @param value Key value
     * @return This object for chaining builder methods
     */
    LinkInfo& doAddProperty(const char *name, const std::string& value);

    /**
     * Add an int value property to the set.
     * @param name Key name
     * @param value Key value
     * @return This object for chaining builder methods
     */
    LinkInfo& doAddProperty(const char *name, int value);

     /**
     * Appends query parameters to the URI string.
     * @param query Input URI string.
     * @param tag tag of the query parameter
     * @param value valueof the query parameter
     */
     void appendQueryParameters(std::string& query, const char* tag, std::string value) const;

 private:
    std::string getAlias() const;
    std::string getCampaign() const;
    std::string getChannel() const;
    std::string getFeature() const;
    std::string getStage() const;
    void complete();

 private:
    std::mutex mutable _mutex;
    std::condition_variable mutable _completeCondition;
    std::thread _thread;
    bool volatile _complete;
    bool volatile _canceled;
    PropertyManager _controlParams;
    std::vector<std::string> _tagParams;

    // Store values passed to createUrl() for fallback handling.
    IRequestCallback* volatile _callback;
    Branch* volatile _branch;

    IClientSession* _clientSession;
};

}  // namespace BranchIO

#endif  // BRANCHIO_LINKINFO_H__
