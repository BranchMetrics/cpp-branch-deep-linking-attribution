// Copyright (c) 2019 Branch Metrics, Inc.

#include "BranchIO/JSONObject.h"

#include <Poco/JSON/Parser.h>
#include <Poco/StreamCopier.h>

#include <fstream>
#include <iostream>

using namespace std;
using namespace Poco;
using namespace Poco::Dynamic;
using namespace Poco::JSON;

namespace BranchIO {

JSONObject::JSONObject(const Poco::JSON::Object& pocoObject)
    : Poco::JSON::Object(pocoObject) {
}

bool
JSONObject::isEmpty() const {
    return size() == 0;
}

JSONObject
JSONObject::parse(const std::string& jsonString) {
    Parser parser;
    Var result = parser.parse(jsonString);
    Object::Ptr resultPtr = result.extract<Object::Ptr>();
    return JSONObject(*resultPtr);
}

JSONObject
JSONObject::parse(std::istream& s) {
    ostringstream oss;
    StreamCopier::copyStream(s, oss);
    return parse(oss.str());
}

JSONObject
JSONObject::load(const std::string& path) {
    // std::ifstream constructor throws std::runtime_error if file does not exist, etc.
    ifstream jsonFile(path.c_str());
    return parse(jsonFile);
}

std::string
JSONObject::stringify(unsigned int indent, int step) const {
    ostringstream ss;
    stringify(ss, indent, step);
    return ss.str();
}

JSONObject &
JSONObject::operator += (const JSONObject &rhs) {
    for (JSONObject::ConstIterator it = rhs.begin(); it != rhs.end(); ++it) {
        set(it->first, it->second);
    }
    return *this;
}

void
JSONObject::set(const std::string& key, JSONObject::Ptr ptr) {
    if (ptr.isNull()) {
        Object::remove(key);
    } else {
        Object &object(*ptr.get());
        Object::set(key, object);
    }
}

void
JSONObject::set(const std::string& key, Poco::JSON::Array::Ptr ptr) {
    if (ptr.isNull()) {
        Object::remove(key);
    } else {
        JSON::Array &array(*ptr.get());
        Object::set(key, array);
    }
}

}  // namespace BranchIO
