// Copyright (c) 2019-21 Branch Metrics, Inc.

#include <cassert>
#include <vector>
#include "BranchIO/Util/WindowsStorage.h"
#include "BranchIO/Util/Log.h"

using namespace std;

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

    string rootPath = rootSubPath;

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

HKEY WindowsStorage::getRegistryHandle(Scope scope) const
{
    HKEY hKey = 0;
    
    if (scope == Default) scope = getDefaultScope();
    
    // default scope must be set, or non-Default value passed in
    assert(scope != Default);

    switch (scope) {
    case Host:
        hKey = HKEY_LOCAL_MACHINE;
        break;
    case User:
        hKey = HKEY_CURRENT_USER;
        break;
    }
    return hKey;
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
    return registryKeyExists(registryKey, registryPath, scope) ||
            registryKeyExists(registryKey + "\\" + registryPath, "", scope);
}

bool
WindowsStorage::registryKeyExists(const std::string registryKey, const std::string registryValue, Scope scope) const {

    BOOL bExist = FALSE;
    HKEY hKey;
    HKEY hRootKey = getRegistryHandle(scope);

    if (RegOpenKeyExA(hRootKey, registryKey.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {   
        if (!registryValue.empty())
        {
            if (RegQueryValueExA(hKey, registryValue.c_str(), NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
            {
                bExist = TRUE;
            }
        }
        else
        {
            bExist = TRUE;
        }
        RegCloseKey(hKey);
    }
    else
    {
        BRANCH_LOG_D("Not a valid registry key - " << hRootKey << "\\" << registryKey);
    }
    return bExist;
}

std::string
WindowsStorage::getString(const std::string& key, const std::string& defaultValue, Scope scope) const {
    string registryKey, registryPath;
    bool validKey(getRegistryKeyAndPath(scope, key, registryKey, registryPath));
    assert(validKey);

    DWORD dwBufSize = 0;
    LONG lRetVal = RegGetValueA(getRegistryHandle(scope),
        registryKey.c_str(),
        registryPath.c_str(),
        RRF_RT_REG_SZ,
        NULL,
        NULL,
        &dwBufSize);
    
    if (ERROR_SUCCESS != lRetVal)
        return defaultValue;

    // If value is not empty - allocate buffer and read value
    if (dwBufSize > 0)
    {
        std::string valueBuf;
        valueBuf.resize(dwBufSize);

        lRetVal = RegGetValueA(getRegistryHandle(scope),
            registryKey.c_str(),
            registryPath.c_str(),
            RRF_RT_REG_SZ,
            NULL,
            static_cast<void*>(valueBuf.data()),
            &dwBufSize);
        if (ERROR_SUCCESS != lRetVal)
        {
            BRANCH_LOG_D("Reading from registry failed. Windows system error code: " << lRetVal);
            return defaultValue;

        }
        string keyValue = string(valueBuf.data(), dwBufSize - 1); // Removing terminating Null bytes "/0/0" as type is RRF_RT_REG_SZ.
        return keyValue;
    }

    return defaultValue;
}

IStorage&
WindowsStorage::setString(const std::string& key, const std::string& value, Scope scope) {
    string registryKey, registryPath;
    bool validKey(getRegistryKeyAndPath(scope, key, registryKey, registryPath));
    assert(validKey);

    LONG lRetVal = RegSetKeyValueA(getRegistryHandle(scope), registryKey.c_str(), registryPath.c_str(), REG_SZ, value.c_str(), (DWORD)value.length()+1);
    
    if (ERROR_SUCCESS != lRetVal)
    {
        BRANCH_LOG_D("Writing to registry failed. Windows system error code: " << lRetVal);
    }
    return *this;
}

bool
WindowsStorage::getBoolean(const std::string& key, bool defaultValue, Scope scope) const {
    string registryKey, registryPath;
    bool validKey(getRegistryKeyAndPath(scope, key, registryKey, registryPath));
    assert(validKey);

    DWORD value;
    DWORD size = sizeof(value);

    LONG lRetVal = RegGetValueA(getRegistryHandle(scope),
        registryKey.c_str(),
        registryPath.c_str(),
        RRF_RT_REG_DWORD,
        NULL,
        (void*)&value,
        &size);

    if (ERROR_SUCCESS != lRetVal)
    {
        BRANCH_LOG_D("Reading from registry failed. Windows system error code: " << lRetVal);
        return defaultValue;
    }

    return (value == 0 ? false : true);
}

IStorage&
WindowsStorage::setBoolean(const std::string& key, bool value, Scope scope) {
    string registryKey, registryPath;
    bool validKey(getRegistryKeyAndPath(scope, key, registryKey, registryPath));
    assert(validKey);

    DWORD regValue = (value ? 1 : 0);

    LONG lRetVal = RegSetKeyValueA(getRegistryHandle(scope), registryKey.c_str(), registryPath.c_str(), REG_DWORD, (const BYTE*)&regValue, sizeof(regValue));
   
    if (ERROR_SUCCESS != lRetVal)
    {
        BRANCH_LOG_D("Writing to registry failed. Windows system error code: " << lRetVal);
    }
    return *this;
}

bool
WindowsStorage::remove(const std::string& key, Scope scope) {
    string registryKey, registryPath;
    bool validKey(getRegistryKeyAndPath(scope, key, registryKey, registryPath));
    assert(validKey);
  
    if (registryKeyExists(registryKey, registryPath, scope))
        deleteRegKeyAndPath(registryKey, registryPath, scope);
    else if (registryKeyExists(registryKey + "\\" + registryPath, "", scope))
        deleteRegKeyAndPath(registryKey + "\\" + registryPath, "", scope);
    else
        return false;

    return true;
}

bool
WindowsStorage::deleteRegKeyAndPath(const std::string& key, const std::string& path, Scope scope) {
    
    HKEY hKey;
    
    LONG lRetVal = RegOpenKeyExA(getRegistryHandle(scope), key.c_str(), 0, KEY_SET_VALUE, &hKey);
    
    if (lRetVal  == ERROR_SUCCESS) {

        lRetVal = RegDeleteValueA(hKey, path.c_str());
        if (ERROR_SUCCESS != lRetVal)
        {
            BRANCH_LOG_D("RegDeleteValue failed." << key << path << "Windows system error code: " << lRetVal);
        }
        RegCloseKey(hKey);
    }

    return ((lRetVal == ERROR_SUCCESS) ? true : false);
}

void WindowsStorage::deleteRegKey(const std::string& key, Scope scope) {

    LONG lRetVal = RegDeleteKeyA(getRegistryHandle(scope), key.c_str());
    if (ERROR_SUCCESS != lRetVal){
        BRANCH_LOG_D("RegDeleteKey failed." << key << "Windows system error code: " << lRetVal);
    }
    return;
}

IStorage&
WindowsStorage::clear(Scope scope) {
    if (scope == Default) scope = getDefaultScope();
    assert(scope != Default);

    string registryKey = rootSubPath;

    string prefix = getPrefix();
    if (!prefix.empty()) {
        registryKey += string("\\") + prefix;
    }

    deleteRegKey(registryKey, scope);
    return *this;
}

}  // namespace BranchIO
