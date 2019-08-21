// Copyright (c) 2019 Branch Metrics, Inc.

#include "BranchIO/JSONObject.h"

#include <Poco/JSON/Parser.h>
#include <Poco/StreamCopier.h>

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;
using namespace Poco;
using namespace Poco::Dynamic;
using namespace Poco::JSON;

namespace BranchIO {

JSONObject::JSONObject(const Poco::JSON::Object& pocoObject)
    : Poco::JSON::Object(pocoObject) {
}

JSONObject::Ptr
JSONObject::parse(const std::string& jsonString) {
    Parser parser;

    try {
        Var result = parser.parse(jsonString);
        Object::Ptr resultPtr = result.extract<Object::Ptr>();

        // We have to return an actual JSONObject.
        // The Poco::JSON::Object::parse method returns instances of the
        // base class. Use the conversion operator.
        return new JSONObject(*resultPtr);
    }
    catch(Poco::JSON::JSONException &e) {
        cerr << "Error: " << e.message() << endl;
        // @todo(jdee): Should this method actually log?
        // Maybe we should remove the try/catch and let callers
        // decide how to handle it.
        // Anyway, with the logging, we rethrow.
        throw;
    }
}

JSONObject::Ptr
JSONObject::parse(std::istream& s) {
    ostringstream oss;
    StreamCopier::copyStream(s, oss);
    return parse(oss.str());
}

JSONObject
JSONObject::load(const std::string& path) {
    // std::ifstream constructor throws std::runtime_error if file does not exist, etc.
    ifstream jsonFile(path.c_str());
    return *parse(jsonFile);
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
JSONObject::set(const std::string& key, Ptr ptr) {
    Object& object(*ptr.get());
    Object::set(key, object);
}

}  // namespace BranchIO
