// Copyright (c) 2019 Branch Metrics, Inc.

#ifndef BRANCHIO_JSONOBJECT_H__
#define BRANCHIO_JSONOBJECT_H__

#include <Poco/JSON/Object.h>
#include <Poco/JSON/Array.h>

#include <iosfwd>
#include <string>

#include "BranchIO/dll.h"

namespace BranchIO {

/**
 * A representation of a JSON Object.
 */
class BRANCHIO_DLL_EXPORT JSONObject : public Poco::JSON::Object {
 public:
    /**
     * Conversion operator from base class and default constructor.
     * Initializes a JSONObject wrapping the same Poco object, which
     * may or may not be a BranchIO::JSONObject.
     * @param object a JSON object to copy
     */
    JSONObject(
        const Poco::JSON::Object& object = Poco::JSON::Object());

    /**
     * Override the definition from the base class so that we get
     * a JSONObject* instead of a Poco::JSON::Object*.
     */
    typedef Poco::SharedPtr<JSONObject> Ptr;

    /**
     * Parse a JSON formatted string.
     * @param jsonString JSON formatted string
     * @return a new JSONObject Ptr.
     * @throw Poco::JSON::JSONException in case of parse failure.
     */
    static JSONObject parse(const std::string& jsonString);

    /**
     * Parse a JSON formatted stream.
     * @param s JSON formatted stream
     * @return a new JSONObject Ptr.
     * @throw Poco::JSON::JSONException in case of parse failure.
     */
    static JSONObject parse(std::istream& s);

    /**
     * Load a JSON object from a file. Useful for fixtures in testing.
     * @param path file path to load
     * @return a JSONObject containing the contents of the file
     * @throw std::runtime_error in case of failure to read the file
     * @throw Poco::JSON::JSONException in case of parse failure.
     */
    static JSONObject load(const std::string& path);

    /**
     * Indicate whether an instance is empty.
     * @return true if empty, false otherwise
     */
    bool isEmpty() const;

    /**
     * Represent this class as a string.
     * When indent is 0, the string will be created as small as possible.
     * Indentation is increased/decreased using number of spaces defined in step.
     * The default value -1 for step indicates that step will be equal to the indent size.
     * @param indent indent level
     * @param step step size
     * @return a string representation of this class.
     */
    std::string stringify(unsigned int indent = 0, int step = -1) const;

    /**
     * Operator +=
     * @param rhs Other to combine with this object.
     * @return this object.
     * @todo(andyp): Revisit how to combine JSONObjects.
     */
    JSONObject & operator += (const JSONObject &rhs);

    // explicitly bring in the base class methods with the same
    // names (overloads)
    using Poco::JSON::Object::stringify;
    using Poco::JSON::Object::set;

    /**
     * Overload of set to take a JSONObject::Ptr.
     * @param key Key
     * @param ptr Value
     */
    void set(const std::string& key, JSONObject::Ptr ptr);

    /**
     * Overload of set to take a JSONArray::Ptr.
     * @param key Key
     * @param ptr Value
     */
    void set(const std::string& key, Poco::JSON::Array::Ptr ptr);

    /**
     * Overload of set to take a reference to a JSONObject.
     * @param key Key
     * @param object Value
     */
    void set(const std::string& key, const JSONObject& object) {
        // This requires a static_cast to the base class in order to pass it to the
        // appropriate overload in the base class.
        Poco::JSON::Object::set(key, static_cast<Poco::JSON::Object const&>(object));
    }

    /**
     * Extract a subobject
     * @todo(jdee): Might want to make this a template function to extract
     * other types, or otherwise rethink this.
     * @param key the key to extract
     * @return the value of the subobject
     */
    JSONObject extract(const std::string& key) const {
        return get(key).extract<Poco::JSON::Object>();
    }
};

}  // namespace BranchIO

#endif  // BRANCHIO_JSONOBJECT_H__
