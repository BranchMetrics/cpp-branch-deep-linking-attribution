// Copyright (c) 2019-21 Branch Metrics, Inc.

#include <Poco/Util/WinRegistryKey.h>

#include <cassert>
#include <vector>

#include "BranchIO/Util/WindowsStorage.h"

using namespace std;
using namespace Poco;
using namespace Poco::Util;

namespace BranchIO {

IStorage&
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

    string prefix = getPrefix();
    if (!prefix.empty()) {
        rootPath += string("\\") + prefix;
    }

    // Glue everything together into one long root\a\b\c
    string path = rootPath + "\\" + convertKey(key);

    // Split the root\a\b  and \c
    string regKey, regPath;
    if (!splitRegistryKeyAndPath(path, regKey, regPath)) return false;

    registryPath = regPath;
    registryKey = regKey;

    return true;
}

bool
WindowsStorage::splitRegistryKeyAndPath(const std::string& key, std::string& registryKey, std::string& registryPath) {
    // split off the last component, e.g. key="a\b\c" -> registryKey="a\b", registryPath="c"
    // fails if not at least two components

    string::size_type lastDelim = key.find_last_of("\\");
    if (lastDelim == string::npos) return false;

    registryKey = key.substr(0, lastDelim);
    registryPath = key.substr(lastDelim + 1);  // to end of string

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
    scoped_lock _l(_mutex);
    return _defaultScope;
}

IStorage&
WindowsStorage::setDefaultScope(Scope scope) {
    scoped_lock _l(_mutex);
    _defaultScope = scope;
    return *this;
}

std::string
WindowsStorage::getPrefix() const {
    scoped_lock _l(_mutex);
    return _prefix;
}

IStorage&
WindowsStorage::setPrefix(const std::string& prefix) {
    scoped_lock _l(_mutex);
    _prefix = prefix;
    return *this;
}

bool
WindowsStorage::has(const std::string& key, Scope scope) const {
    string registryKey, registryPath;
    bool validKey(getRegistryKeyAndPath(scope, key, registryKey, registryPath));
    assert(validKey);

    // Return true if a key or leaf exists at this path
    return
        WinRegistryKey(registryKey).exists(registryPath) ||
        WinRegistryKey(registryKey + "\\" + registryPath).exists();
}

std::string
WindowsStorage::getString(const std::string& key, const std::string& defaultValue, Scope scope) const {
    string registryKey, registryPath;
    bool validKey(getRegistryKeyAndPath(scope, key, registryKey, registryPath));
    assert(validKey);

    if (!has(key, scope)) return defaultValue;

    return WinRegistryKey(registryKey).getString(registryPath);
}

IStorage&
WindowsStorage::setString(const std::string& key, const std::string& value, Scope scope) {
    string registryKey, registryPath;
    bool validKey(getRegistryKeyAndPath(scope, key, registryKey, registryPath));
    assert(validKey);

    WinRegistryKey(registryKey).setString(registryPath, value);

    return *this;
}

bool
WindowsStorage::getBoolean(const std::string& key, bool defaultValue, Scope scope) const {
    string registryKey, registryPath;
    bool validKey(getRegistryKeyAndPath(scope, key, registryKey, registryPath));
    assert(validKey);

    if (!has(key, scope)) return defaultValue;

    int v = WinRegistryKey(registryKey).getInt(registryPath);
    return (v == 0 ? false : true);
}

IStorage&
WindowsStorage::setBoolean(const std::string& key, bool value, Scope scope) {
    string registryKey, registryPath;
    bool validKey(getRegistryKeyAndPath(scope, key, registryKey, registryPath));
    assert(validKey);

    WinRegistryKey(registryKey).setInt(registryPath, (value ? 1 : 0));

    return *this;
}

bool
WindowsStorage::remove(const std::string& key, Scope scope) {
    string registryKey, registryPath;
    bool validKey(getRegistryKeyAndPath(scope, key, registryKey, registryPath));
    assert(validKey);

    if (!has(key, scope)) return false;

    if (WinRegistryKey(registryKey).exists(registryPath))
        WinRegistryKey(registryKey).deleteValue(registryPath);
    else if (WinRegistryKey(registryKey + "\\" + registryPath).exists())
        WinRegistryKey(registryKey + "\\" + registryPath).deleteKey();

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

    string prefix = getPrefix();
    if (!prefix.empty()) {
        registryKey += string("\\") + prefix;
    }

    WinRegistryKey(registryKey).deleteKey();
    return *this;
}

}  // namespace BranchIO
