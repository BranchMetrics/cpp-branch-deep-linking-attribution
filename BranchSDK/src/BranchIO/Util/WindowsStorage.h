// Copyright (c) 2019-21 Branch Metrics, Inc.

#ifndef BRANCHIO_UTIL_WINDOWSSTORAGE_H_
#define BRANCHIO_UTIL_WINDOWSSTORAGE_H_

#include <string>

#include "BranchIO/Util/IStorage.h"
#include <mutex>

namespace BranchIO {

/**
 * Windows Registry-based storage utility
 */
class WindowsStorage : public virtual IStorage {
 public:
    /**
     * Root sub path 
     */
    static constexpr const char* const rootSubPath = "SOFTWARE\\BranchIO";

    /**
     * Singleton accessor
     * @return the single WindowsStorage instance
     */
    static IStorage& instance();

    /**
     * @copydoc IStorage::getDefaultScope
     */
    Scope getDefaultScope() const;

    /**
     * @copydoc IStorage::setDefaultScope
     */
    IStorage& setDefaultScope(Scope scope);

    std::string getPrefix() const;
    IStorage& setPrefix(const std::string& prefix);

    /**
     * @copydoc IStorage::has
     */
    bool has(const std::string& key, Scope scope = Default) const;

    /**
     * @copydoc IStorage::getString
     */
    std::string getString(const std::string& key, const std::string& defaultValue = "", Scope scope = Default) const;

    /**
     * @copydoc IStorage::setString
     */
    IStorage& setString(const std::string& key, const std::string& value, Scope scope = Default);

    /**
     * @copydoc IStorage::getBoolean
     */
    bool getBoolean(const std::string& key, bool defaultValue = false, Scope scope = Default) const;

    /**
     * @copydoc IStorage::setBoolean
     */
    IStorage& setBoolean(const std::string& key, bool value, Scope scope = Default);

    /**
     * @copydoc IStorage::remove
     */
    bool remove(const std::string& key, Scope scope = Default);

    /**
     * @copydoc IStorage::clear
     */
    IStorage& clear(Scope scope = Default);

 private:
    static std::string convertKey(const std::string& key);
    static bool splitRegistryKeyAndPath(
        const std::string& key,
        std::string& registryKey,
        std::string& registryPath);

    WindowsStorage();
    WindowsStorage(const WindowsStorage& o);
    ~WindowsStorage();
    WindowsStorage& operator=(const WindowsStorage& o) { return *this; }

    bool getRegistryKeyAndPath(
        Scope scope,
        const std::string& key,
        std::string& registryKey,
        std::string& registryPath) const;

    HKEY getRegistryHandle(Scope scope) const;
    bool registryKeyExists(const std::string registryKey, const std::string registryValue, Scope scope) const;
    bool deleteRegKeyAndPath(const std::string& key, const std::string& path, Scope scope);
    void deleteRegKey(const std::string& key, Scope scope);

    mutable std::mutex _mutex;
    Scope _defaultScope;
    std::string _prefix;
};

}  // namespace BranchIO

#endif  // BRANCHIO_UTIL_WINDOWSSTORAGE_H_
