// Copyright (c) 2019-21 Branch Metrics, Inc.

#ifndef BRANCHIO_JSONOBJECT_H__
#define BRANCHIO_JSONOBJECT_H__

#include <iosfwd>
#include <string>
#include "BranchIO/dll.h"
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Foundation.Collections.h>

class PropertyManager;

namespace BranchIO {

/**
 * A representation of a JSON Object.
 */
class BRANCHIO_DLL_EXPORT JSONObject  {
 public:

     JSONObject();

     /**
     * Conversion operator from base class and default constructor.
     * Initializes a JSONObject wrapping the same WinRT JsonObject.
     * @param object a JSON object to copy
     */
     JSONObject(const winrt::Windows::Data::Json::JsonObject& object);

    /**
     * Pointer to JSONObject*
     */
    typedef std::shared_ptr<JSONObject> Ptr;

    /**
     * Parse a JSON formatted string.
     * @param jsonString JSON formatted string
     * @return a new JSONObject.
     * @throw winrt::hresult_error in case of parse failure.
     */
    static JSONObject parse(const std::string& jsonString);

    /**
     * Parse a JSON formatted stream.
     * @param s JSON formatted stream
     * @return a new JSONObject Ptr.
     * @throw winrt::hresult_error in case of parse failure.
     */
    static JSONObject parse(std::istream& s);

    /**
     * Load a JSON object from a file. Useful for fixtures in testing.
     * @param path file path to load
     * @return a JSONObject containing the contents of the file
     * @throw std::runtime_error in case of failure to read the file
     * @throw winrt::hresult_error in case of parse failure.
     */
    static JSONObject load(const std::string& path);

    /**
     * Indicate whether an instance is empty.
     * @return true if empty, false otherwise
     */
    bool isEmpty() const;

    /**
     * Represent this class as a string.
     * @return a string representation of this class.
     */
    std::string stringify() const;

    /**
     * Operator +=
     * @param rhs Other to combine with this object.
     * @return this object.
     * @todo(andyp): Revisit how to combine JSONObjects.
     */
    JSONObject & operator += (const JSONObject &rhs);

    /**
     *  set - sets values for the specified key in the JSON Object.
     * @param key Key
     * @param value Value 
     */
    void set(const std::string& key, const std::string& value);
    void set(const std::string& key, const int& value);
    void set(const std::string& key, const double& value);
    void set(const std::string& key, const JSONObject value) const;
    void set(const std::string& key, const std::vector<std::string> value) const;
    void set(const JSONObject& jsonObject) const;

    /**
    * getNamedString - Gets the String value with the specified name(key) in the JSON Object.
    * @param name - The name/key
    * @return string value for the key
    */
    std::string getNamedString(std::string const& name) const;

    /**
    * has - Indicates whether the JsonObject has an entry with the requested key.
    * @param key - The key
    * @return true if the JsonObject has an entry with the requested key; otherwise, false.
    */
    bool has(const std::string& key) const;

    /**
    * remove - Removes a specific item from the JsonObject.
    * @param key - The key of the item to remove.
    */
    void remove(const std::string& key);
    
    /**
    * getWinRTJsonObj - Returns the encapsulated WinRT JsonObject.
    * @return encapsulated WinRT JsonObject
    */
    const winrt::Windows::Data::Json::JsonObject getWinRTJsonObj() const;
    
    /**
    * size - Gets the number of items in the JSON Object.
    * @return number of items
    */
    const uint32_t size() const;
    
    /**
    * clear - Removes all items.
    */
    void clear() const;

protected:
    winrt::Windows::Data::Json::JsonObject jObject;
};

}  // namespace BranchIO

#endif  // BRANCHIO_JSONOBJECT_H__
