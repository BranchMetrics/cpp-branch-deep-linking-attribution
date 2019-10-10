// Copyright (c) 2019 Branch Metrics, Inc.

#ifndef BRANCHIO_LINKINFO_H__
#define BRANCHIO_LINKINFO_H__

#include <string>
#include "BranchIO/PropertyManager.h"

namespace BranchIO {

/**
 * Link Information.
 */
class BRANCHIO_DLL_EXPORT LinkInfo : public PropertyManager {
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
     * Constructor.
     * @param jsonObject JSON Object to pre-initialize with
     */
    explicit LinkInfo(const JSONObject &jsonObject);
    virtual ~LinkInfo();

    /**
     * Any other params to be added; you can define your own.
     *
     * @param key   A string with the key for the parameter
     * @param value A string with the value for the parameter
     * @return This object for chaining builder methods
     */
    virtual LinkInfo& addControlParameter(const char *key, const std::string &value);
    virtual LinkInfo& addControlParameter(const char *key, int value);
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
    PropertyManager _controlParams;
};

}  // namespace BranchIO

#endif  // BRANCHIO_LINKINFO_H__
