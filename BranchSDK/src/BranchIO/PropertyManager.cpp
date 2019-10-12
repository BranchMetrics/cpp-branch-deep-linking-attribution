// Copyright (c) 2019 Branch Metrics, Inc.

#include "BranchIO/PropertyManager.h"
#include "BranchIO/JSONObject.h"

using namespace Poco;

namespace BranchIO {

PropertyManager::PropertyManager() {
}

PropertyManager::PropertyManager(const JSONObject &jsonObject)
    : JSONObject(jsonObject) {
}

PropertyManager::PropertyManager(const PropertyManager &other) :
    JSONObject(other) {
}

PropertyManager&
PropertyManager::operator=(const PropertyManager& other) {
    return static_cast<PropertyManager&>(JSONObject::operator=(other));
}

PropertyManager::~PropertyManager() {
}

PropertyManager&
PropertyManager::addProperty(const char *name, const std::string &value) {
    Mutex::ScopedLock _l(_mutex);

    if (value.length() == 0) {
        remove(name);
    } else {
        set(name, value);
    }
    return *this;
}

PropertyManager&
PropertyManager::addProperty(const char *name, int value) {
    Mutex::ScopedLock _l(_mutex);

    set(name, value);
    return *this;
}

PropertyManager&
PropertyManager::addProperty(const char *name, double value) {
    Mutex::ScopedLock _l(_mutex);

    set(name, value);
    return *this;
}

PropertyManager&
PropertyManager::addProperty(const char *name, const PropertyManager &value) {
    Mutex::ScopedLock _l(_mutex);

    set(name, value);
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

std::string
PropertyManager::getStringProperty(const char *name) {
    Mutex::ScopedLock _l(_mutex);

    return getValue<std::string>(name);
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

}  // namespace BranchIO
