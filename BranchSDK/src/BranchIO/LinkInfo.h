// Copyright (c) 2019 Branch Metrics, Inc.

#ifndef BRANCHIO_LINKINFO_H__
#define BRANCHIO_LINKINFO_H__

#include <string>
#include "BranchIO/Branch.h"
#include "BranchIO/Event/Event.h"

namespace BranchIO {

/**
 * Link Information.
 */
class BRANCHIO_DLL_EXPORT LinkInfo : protected Event {
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

    virtual ~LinkInfo();

    /**
     * Any other params to be added; you can define your own.
     *
     * @param key   A string with the key for the parameter
     * @param value A string with the value for the parameter
     * @return This object for chaining builder methods
     */
    virtual LinkInfo& addControlParameter(const char *key, const std::string &value);

    /**
     * Any other params to be added; you can define your own.
     *
     * @param key   A string with the key for the parameter
     * @param value An integer with the value for the parameter
     * @return This object for chaining builder methods
     */
    virtual LinkInfo& addControlParameter(const char *key, int value);

    /**
     * Any other params to be added; you can define your own.
     *
     * @param key   A string with the key for the parameter
     * @param value A PropertyManager with the value for the parameter
     * @return This object for chaining builder methods
     */
    virtual LinkInfo& addControlParameter(const char *key, const PropertyManager &value);

    /**
     * Adds a tag to the collection associated with a deep link.
     *
     * @param tag A tag associated with a deep link.
     * @return This object for chaining builder methods
     */
    virtual LinkInfo&  addTag(const std::string &tag);

    /**
     * Sets the alias for this link.
     *
     * @param alias A string value containing the desired alias name to add.
     * @return This object for chaining builder methods
     */
    virtual LinkInfo& setAlias(const std::string &alias);

    /**
     * A string value that represents the campaign associated with this link.
     *
     * @param campaign A string value specifying the campaign.
     * @return This object for chaining builder methods
     */
    virtual LinkInfo& setCampaign(const std::string &campaign);

    /**
     * [Optional] The channel in which the link will be shared. eg: "facebook",
     * "text_message".
     *
     * @param channel A string value containing the channel which the link
     *                belongs to. (max 128 characters).
     * @return This object for chaining builder methods
     */
    virtual LinkInfo& setChannel(const std::string &channel);

    /**
     * [Optional] You can set the duration manually. This is the time that
     * Branch allows a click to remain outstanding and be eligible to be matched
     * with a new app session.
     *
     * @param duration An integer value in seconds.
     * @return This object for chaining builder methods
     */
    virtual LinkInfo& setDuration(int duration);

    /**
     * [Optional] The feature in which the link will be used. eg: "invite",
     * "referral", "share", "gift", etc.
     *
     * @param feature A string specifying the feature. (max 128 characters).
     * @return This object for chaining builder methods
     */
    virtual LinkInfo& setFeature(const std::string &feature);

    /**
     * A string value that represents the stage of the user in the app. eg:
     * "level1", "logged_in", etc.
     *
     * @param stage A string value specifying the stage.
     * @return This object for chaining builder methods
     */
    virtual LinkInfo& setStage(const std::string &stage);

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
    virtual LinkInfo& setType(int type);

    /**
     * Generate a Branch Url with the given deep link parameters and link properties.
     * Note that this will "fall back" to generating a Long URL on error.
     * Note that if the callback is null, no request will be made.
     * @param branchInstance Branch Instance
     * @param callback Callback to fire with success or failure notification.
     */
     virtual void generateUrl(Branch *branchInstance, IRequestCallback *callback);

    /**
     * Generate a long Url with the given deep link parameters and link properties.
     * Note that this does not require an active network connection.
     * @param branchKey Branch Key.
     * @param baseUrl Non-Default base to use for forming the url
     * @return A url with the given deep link parameters.
     */
    virtual std::string generateLongUrl(const std::string &branchKey, const std::string &baseUrl = "") const;

    using PropertyManager::toString;

 private:
    /**
     * Add a string value property to the set.
     * Note that if the value is empty, this effectively removes the key.
     * @param name Key name
     * @param value Key value
     * @return This object for chaining builder methods
     */
    LinkInfo& doAddProperty(const char *name, const std::string &value);

    /**
     * Add a int value property to the set.
     * @param name Key name
     * @param value Key value
     * @return This object for chaining builder methods
     */
    LinkInfo& doAddProperty(const char *name, int value);

 private:
    std::string getAlias() const;
    std::string getCampaign() const;
    std::string getChannel() const;
    std::string getFeature() const;
    std::string getStage() const;


 private:
    PropertyManager _controlParams;
    JSONArray _tagParams;
};

}  // namespace BranchIO

#endif  // BRANCHIO_LINKINFO_H__
