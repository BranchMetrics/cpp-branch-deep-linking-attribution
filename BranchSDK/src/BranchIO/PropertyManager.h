// Copyright (c) 2019 Branch Metrics, Inc.

#ifndef BRANCHIO_PROPERTYMANAGER_H__
#define BRANCHIO_PROPERTYMANAGER_H__

#include <Poco/Mutex.h>

#include <string>

#include "BranchIO/Util/IStringConvertible.h"
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
     * Destructor.
     */
    virtual ~PropertyManager();

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

    /**
     * @return this object as a string
     */
    virtual std::string toString() const;

    /**
     * @return this object formatted as a JSONObject.
     * @todo(andyp): Revisit Scope
     */
    virtual JSONObject toJSON() const;

    /**
     * Add a string value property to the set.
     * Note that if the value is empty, this effectively removes the key.
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
     * Add Properties from an existing PropertyManager.
     * @param name Key name
     * @param value Key value
     * @return This object for chaining builder methods
     */
    virtual PropertyManager& addProperty(const char *name, const PropertyManager &value);

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

    /**
     * Retrieve a string property.
     * @param name Key name
     * @return a string property.
     */
    std::string getStringProperty(const char *name);

    /**
     * Load a string property from storage.
     * @param path Path from which this key will be loaded
     * @param key Key to load
     * @param defValue Default value if not found
     * @return String from storage, or default value if not found.
     */
    std::string loadString(const char *path, const char* key, const std::string &defValue);

    /**
     * Save a property to storage.
     * @param path Path to which this key will be saved
     * @param key Key name
     * @param value Key value
     */
    void saveString(const char *path, const char *key, const std::string &value);

    /**
     * Load a string property from storage.
     * @param path Path from which this key will be loaded
     * @param key Key to load
     * @param defValue bool value from storage, or default value if not found.
     * @return true if the value was found
     */
    bool loadBoolean(const char *path, const char* key, bool defValue);

    /**
     * Save a property to storage.
     * @param path Path to which this key will be saved
     * @param key Key name
     * @param value Key value
     */
    void saveBoolean(const char *path, const char *key, bool value);

    /**
     * @param name Key Value
     * @return true when the given property exists
     */
    virtual bool has(const char *name) const;

 private:
    std::string getStoragePath(const char *path, const char* key) const;
    mutable Poco::Mutex _mutex;
};

}  // namespace BranchIO

#endif  // BRANCHIO_PROPERTYMANAGER_H__
