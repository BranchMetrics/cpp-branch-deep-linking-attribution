// Copyright (c) 2019 Branch Metrics, Inc.

#ifndef BRANCHIO_PROPERTYMANAGER_H__
#define BRANCHIO_PROPERTYMANAGER_H__

#include <Poco/Mutex.h>

#include <string>

#include "BranchIO/IStringConvertible.h"
#include "BranchIO/JSONObject.h"

namespace BranchIO {

/**
 * A Class for holding Key/Value pair properties.
 */
class BRANCHIO_DLL_EXPORT PropertyManager : protected JSONObject, public virtual IStringConvertible {
 public:
    /**
     * Constructor.
     */
    PropertyManager();

    /**
     * Constructor.
     * @param jsonObject pre-filled JSONObject to seed this class.
     */
    PropertyManager(const JSONObject &jsonObject); // NOLINT We want this conversion operator

    /**
     * Copy constructor
     * @param other another PropertyManager instance to copy
     */
    PropertyManager(const PropertyManager& other);

    /**
     * Assignment operator
     * @param other another PropertyManager
     * @return *this
     */
    PropertyManager& operator=(const PropertyManager& other);

    virtual ~PropertyManager();

    /**
     * @return this object as a string
     */
    virtual std::string toString() const;

    /**
     * @return this object formatted as a JSONObject.
     * @todo(andyp): Revisit Scope
     */
    JSONObject toJSON() const;

    /**
     * Add a string value property to the set.
     * Note that if the value is empty, this effecively removes the key.
     * @param name Key name
     * @param value Key value
     * @return This object for chaining builder methods
     */
    virtual PropertyManager& addProperty(const char *name, const std::string &value);

    /**
     * Add a int value property to the set.
     * @param name Key name
     * @param value Key value
     * @return This object for chaining builder methods
     */
    virtual PropertyManager& addProperty(const char *name, int value);

    /**
     * Add a double value property to the set.
     * @param name Key name
     * @param value Key value
     * @return This object for chaining builder methods
     */
    virtual PropertyManager& addProperty(const char *name, double value);

    /**
     * Add Properties from an existing JSON Object to the set.
     * @param jsonObject properties.
     * @return This object for chaining builder methods
     */
    virtual PropertyManager& addProperties(const JSONObject &jsonObject);

    /**
     * Clears the properties of the object.
     * @return This object for chaining builder methods
     */
    virtual PropertyManager& clear();

 private:
    mutable Poco::Mutex _mutex;
};

}  // namespace BranchIO

#endif  // BRANCHIO_PROPERTYMANAGER_H__
