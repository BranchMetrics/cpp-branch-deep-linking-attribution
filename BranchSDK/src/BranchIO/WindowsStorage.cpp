// Copyright (c) 2019 Branch Metrics, Inc.

#include <Poco/Util/WinRegistryKey.h>

#include <cassert>
#include <vector>

#include "BranchIO/WindowsStorage.h"

using namespace std;
using namespace Poco;
using namespace Poco::Util;

namespace BranchIO {

WindowsStorage&
WindowsStorage::instance() {
    static WindowsStorage _instance;
    return _instance;
}

WindowsStorage::WindowsStorage() :
    // Default to User scope for now
    _defaultScope(User) {
}

WindowsStorage::WindowsStorage(const WindowsStorage& o) {
}

WindowsStorage::~WindowsStorage() {
}

bool
WindowsStorage::getRegistryKeyAndPath(
    Scope scope,
    const std::string& key,
    std::string& registryKey,
    std::string& registryPath) const {

    if (scope == Default) scope = getDefaultScope();
    // default scope must be set, or non-Default value passed in
    assert(scope != Default);

    string rootPath;
    switch (scope) {
    case Host:
        rootPath = HostRootPath;
        break;
    case User:
        rootPath = UserRootPath;
        break;
    default:
        return false;  // in case int case to Scope enum
    }

    string regKey, regPath;
    if (!splitRegistryKeyAndPath(key, regKey, regPath)) return false;

    registryKey = rootPath + "\\" + convertKey(regKey);
    registryPath = regPath;

    return true;
}

bool
WindowsStorage::splitRegistryKeyAndPath(const std::string& key, std::string& registryKey, std::string& registryPath) {
    // split off the last component, e.g. key="a.b.c" -> registryKey="a.b", registryPath="c"
    // fails if not at least two components

    string::size_type lastDot = key.find_last_of(".");
    if (lastDot == string::npos) return false;

    registryKey = key.substr(0, lastDot);
    registryPath = key.substr(lastDot + 1);  // to end of string

    return true;
}

std::string
WindowsStorage::convertKey(const std::string& key) {
    // replace all dots with backslashes
    string copy = key;
    string::size_type backslash(string::npos);
    while ((backslash = copy.find(".")) != string::npos) {
        copy.replace(backslash, 1, "\\");
    }
    return copy;
}

IStorage::Scope
WindowsStorage::getDefaultScope() const {
    Mutex::ScopedLock _l(_mutex);
    return _defaultScope;
}

IStorage&
WindowsStorage::setDefaultScope(Scope scope) {
    Mutex::ScopedLock _l(_mutex);
    _defaultScope = scope;
    return *this;
}

bool
WindowsStorage::has(const std::string& key, Scope scope) const {
    string registryKey, registryPath;
    bool validKey(getRegistryKeyAndPath(scope, key, registryKey, registryPath));
    assert(validKey);

    return WinRegistryKey(registryKey).exists(registryPath);
}

bool
WindowsStorage::get(const std::string& key, std::string& value, Scope scope) const {
    string registryKey, registryPath;
    bool validKey(getRegistryKeyAndPath(scope, key, registryKey, registryPath));
    assert(validKey);

    if (!has(key, scope)) return false;

    value = WinRegistryKey(registryKey).getString(registryPath);
    return true;
}

IStorage&
WindowsStorage::set(const std::string& key, const std::string& value, Scope scope) {
    string registryKey, registryPath;
    bool validKey(getRegistryKeyAndPath(scope, key, registryKey, registryPath));
    assert(validKey);

    WinRegistryKey(registryKey).setString(registryPath, value);

    return *this;
}

bool
WindowsStorage::remove(const std::string& key, Scope scope) {
    string registryKey, registryPath;
    bool validKey(getRegistryKeyAndPath(scope, key, registryKey, registryPath));
    assert(validKey);

    if (!has(key, scope)) return false;

    WinRegistryKey(registryKey).deleteValue(registryPath);

    return true;
}

IStorage&
WindowsStorage::clear(Scope scope) {
    if (scope == Default) scope = getDefaultScope();
    assert(scope != Default);

    string registryKey;
    switch (scope) {
    case Host:
        registryKey = HostRootPath;
        break;
    case User:
        registryKey = UserRootPath;
        break;
    default:
        return *this;
    }

    WinRegistryKey(registryKey).deleteKey();
    return *this;
}

}  // namespace BranchIO
