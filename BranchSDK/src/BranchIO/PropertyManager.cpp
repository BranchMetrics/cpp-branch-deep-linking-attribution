// Copyright (c) 2019-21 Branch Metrics, Inc.

#include "BranchIO/PropertyManager.h"
#include "BranchIO/JSONObject.h"
#include "BranchIO/Util/Storage.h"
#include "BranchIO/String.h"

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
    return static_cast<PropertyManager&>(JSONObject::operator=(other));
}

PropertyManager&
PropertyManager::addProperty(const String& name, const String& value) {
    scoped_lock _l(_mutex);

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
    scoped_lock _l(_mutex);

    set(name.str(), value);
    return *this;
}

PropertyManager&
PropertyManager::addProperty(const String& name, double value) {
    scoped_lock _l(_mutex);

    set(name.str(), value);
    return *this;
}

PropertyManager&
PropertyManager::addProperty(const String& name, const PropertyManager &value) {
    scoped_lock _l(_mutex);

    set(name.str(), value);
    return *this;
}

PropertyManager&
PropertyManager::addProperty(const String& name, const vector<string> &value) {
    scoped_lock _l(_mutex);

    set(name.str(), value);
    return *this;
}

PropertyManager&
PropertyManager::addProperties(const JSONObject &jsonObject) {
    scoped_lock _l(_mutex);
    set(jsonObject);
    return *this;
}

PropertyManager&
PropertyManager::clear() {
    scoped_lock _l(_mutex);

    JSONObject::clear();
    return *this;
}

bool
PropertyManager::has(const char *name) const {
    scoped_lock _l(_mutex);

    return JSONObject::has(name);
}

bool
PropertyManager::isEmpty() const {
    scoped_lock _l(_mutex);

    return (JSONObject::size() == 0);
}

string
PropertyManager::getStringProperty(const char *name, const string &defValue) const {
    scoped_lock _l(_mutex);

    if (JSONObject::has(name)) {
        return getNamedString(name);
    }

    return defValue;
}

string
PropertyManager::toString() const {
    scoped_lock _l(_mutex);
    return stringify();
}

JSONObject
PropertyManager::toJSON() const {
    scoped_lock _l(_mutex);
    return *this;
}

string
PropertyManager::getPath(const string& base, const string &key)  {
    string storagePath(base);
    if (!base.empty()) {
        storagePath += ".";
    }
    storagePath += key;

    return storagePath;
}


}  // namespace BranchIO
