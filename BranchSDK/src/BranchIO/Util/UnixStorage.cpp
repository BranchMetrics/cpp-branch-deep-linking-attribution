// Copyright (c) 2019 Branch Metrics, Inc.

#include <Poco/File.h>

#include <cstdlib>

#include "UnixStorage.h"

using namespace std;
using namespace Poco;
using namespace Poco::Util;

namespace BranchIO {

UnixStorage&
UnixStorage::instance() {
    static UnixStorage _instance;
    return _instance;
}

IStorage::Scope
UnixStorage::getDefaultScope() const {
    Mutex::ScopedLock _l(_mutex);
    return _defaultScope;
}

IStorage&
UnixStorage::setDefaultScope(Scope scope) {
    Mutex::ScopedLock _l(_mutex);
    _defaultScope = scope;
    return *this;
}

bool
UnixStorage::has(const std::string& key, Scope scope) const {
    Mutex::ScopedLock _l(_mutex);
    return getConfig(scope)->has(key);
}

bool
UnixStorage::get(const std::string& key, std::string& value, Scope scope) const {
    Mutex::ScopedLock _l(_mutex);
    StoragePtr config(getConfig(scope));
    if (!config->has(key)) return false;
    value = config->getString(key);
    return true;
}

IStorage&
UnixStorage::set(const std::string& key, const std::string& value, Scope scope) {
    Mutex::ScopedLock _l(_mutex);
    getConfig(scope)->setString(key, value);
    flush(scope);
    return *this;
}

bool
UnixStorage::remove(const std::string& key, Scope scope) {
    Mutex::ScopedLock _l(_mutex);
    if (!has(key, scope)) return false;

    getConfig(scope)->remove(key);
    flush(scope);
    return true;
}

IStorage&
UnixStorage::clear(Scope scope) {
    if (scope == Default) scope = getDefaultScope();
    // default scope must be set, or non-Default value passed in
    assert(scope != Default);

    switch (scope) {
    case User:
        _userConfig = nullptr;
    case Host:
        _hostConfig = nullptr;
    default:
        break;
    }

    File file(getPath(scope));
    file.remove(true);  // recursively removes the directory

    return *this;
}

void
UnixStorage::flush(Scope scope) {
    if (scope == Default) scope = getDefaultScope();
    // default scope must be set, or non-Default value passed in
    assert(scope != Default);

    StoragePtr config(getConfig(scope));
    if (config.isNull()) return;
    std::string path(getPath(scope));

    File directory(path);
    if (!directory.exists()) directory.createDirectory();

    bool valid = true;
    if (!directory.canRead()) {
        BRANCH_LOG_E(path << " exists but is not readable");
        valid = false;
    }
    if (!directory.canWrite()) {
        BRANCH_LOG_E(path << " exists but is not writable");
        valid = false;
    }
    if (!directory.isDirectory()) {
        BRANCH_LOG_E(path << " exists but is not a directory");
        valid = false;
    }

    if (!valid) {
       BRANCH_LOG_E("Cannot save properties");
       return;
    }

    std::string fullPath(getFullPath(scope));

    // creates the file if it doesn't exist
    config->save(fullPath);
}

std::string
UnixStorage::getPath(Scope scope) {
    string configPath;
    const char* homeDir(getenv("HOME"));
    assert(homeDir != nullptr);

    switch (scope) {
    case User:
        configPath = homeDir;
        // strip the first char off UserPath, assuming it's ~
        assert(UserPath[0] == '~');
        configPath += string(UserPath).substr(1);
        break;
    case Host:
        configPath = HostPath;
        break;
    default:
        break;  // won't get here
    }

    // BRANCH_LOG_V("configuration path is " << configPath);

    return configPath;
}

std::string
UnixStorage::getFullPath(Scope scope) {
    return getPath(scope) + "/" + FileName;
}

UnixStorage::StoragePtr
UnixStorage::loadConfig(Scope scope) {
    string path(getFullPath(scope));
    File file(path);
    StoragePtr config;
    if (file.exists()) {
        // BRANCH_LOG_I("Loading configuration from " << path);
        config = new PropertyFileConfiguration(path);
    } else {
        config = new PropertyFileConfiguration();
    }

    return config;
}

}  // namespace BranchIO
