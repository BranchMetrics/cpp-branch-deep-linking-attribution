// Copyright (c) 2019 Branch Metrics, Inc.

#include "BranchIO/PropertyManager.h"
#include "BranchIO/JSONObject.h"
#include "BranchIO/Util/Storage.h"
#include "BranchIO/String.h"

using namespace Poco;
using namespace std;

namespace BranchIO {

PropertyManager::PropertyManager() {
}

PropertyManager::PropertyManager(const JSONObject &jsonObject)
    : JSONObject(jsonObject) {
}

PropertyManager::PropertyManager(const PropertyManager &other)
    : JSONObject(other) {
}

PropertyManager&
PropertyManager::operator=(const PropertyManager& other) {
    return dynamic_cast<PropertyManager&>(JSONObject::operator=(other));
}

PropertyManager::~PropertyManager() = default;

PropertyManager&
PropertyManager::addProperty(const String& name, const String& value) {
    Mutex::ScopedLock _l(_mutex);

    string utf8Name(name.str());
    string utf8Val(value.str());
    if (value.str().length() == 0) {
        remove(utf8Name);
    } else {
        set(utf8Name, utf8Val);
    }
    return *this;
}

PropertyManager&
PropertyManager::addProperty(const String& name, int value) {
    Mutex::ScopedLock _l(_mutex);

    set(name.str(), value);
    return *this;
}

PropertyManager&
PropertyManager::addProperty(const String& name, double value) {
    Mutex::ScopedLock _l(_mutex);

    set(name.str(), value);
    return *this;
}

PropertyManager&
PropertyManager::addProperty(const String& name, const PropertyManager &value) {
    Mutex::ScopedLock _l(_mutex);

    set(name.str(), value);
    return *this;
}

PropertyManager&
PropertyManager::addProperty(const String& name, const Poco::JSON::Array &value) {
    Mutex::ScopedLock _l(_mutex);

    set(name.str(), value);
    return *this;
}

PropertyManager&
PropertyManager::addProperties(const JSONObject &jsonObject) {
    Mutex::ScopedLock _l(_mutex);

    for (JSONObject::ConstIterator it = jsonObject.begin(); it != jsonObject.end(); ++it) {
        set(it->first, it->second);
    }

    return *this;
}

PropertyManager&
PropertyManager::clear() {
    Mutex::ScopedLock _l(_mutex);

    JSONObject::clear();
    return *this;
}

bool
PropertyManager::has(const char *name) const {
    Mutex::ScopedLock _l(_mutex);

    return JSONObject::has(name);
}

bool
PropertyManager::isEmpty() const {
    Mutex::ScopedLock _l(_mutex);

    return (JSONObject::size() == 0);
}

std::string
PropertyManager::getStringProperty(const char *name, const std::string &defValue) const {
    Mutex::ScopedLock _l(_mutex);

    if (has(name)) {
        return getValue<std::string>(name);
    }

    return defValue;
}

std::string
PropertyManager::toString() const {
    Mutex::ScopedLock _l(_mutex);

    std::stringstream ss;
    stringify(ss);

    return ss.str();
}

JSONObject
PropertyManager::toJSON() const {
    Mutex::ScopedLock _l(_mutex);
    return *this;
}

std::string
PropertyManager::getPath(const std::string& base, const std::string &key)  {
    std::string storagePath(base);
    if (!base.empty()) {
        storagePath += ".";
    }
    storagePath += key;

    return storagePath;
}


}  // namespace BranchIO
