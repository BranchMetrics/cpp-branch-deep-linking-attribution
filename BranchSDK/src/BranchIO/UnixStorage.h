// Copyright (c) 2019 Branch Metrics, Inc.

#ifndef BRANCHIO_UNIXSTORAGE_H_
#define BRANCHIO_UNIXSTORAGE_H_

#include <Poco/Mutex.h>
#include <Poco/Util/PropertyFileConfiguration.h>

#include <cassert>
#include <string>

#include "BranchIO/IStorage.h"
#include "BranchIO/Util/Log.h"

namespace BranchIO {

/**
 * Unix storage utility (currently implemented with STL maps)
 */
class UnixStorage : public virtual IStorage {
 public:
    /// file name for local storage
    static constexpr const char* const FileName = "branchio.properties";

    /// path for host-level storage
    static constexpr const char* const HostPath = "/usr/local/etc/branchio";

    /// path for user-level storage
    static constexpr const char* const UserPath = "~/.branchio";

    /**
     * Singleton accessor
     * @return the single UnixStorage instance
     */
    static UnixStorage& instance();

    /**
     * @copydoc IStorage::getDefaultScope
     */
    Scope getDefaultScope() const;

    /**
     * @copydoc IStorage::setDefaultScope
     */
    IStorage& setDefaultScope(Scope scope);

    /**
     * @copydoc IStorage::has
     */
    bool has(const std::string& key, Scope scope = Default) const;

    /**
     * @copydoc IStorage::get
     */
    bool get(const std::string& key, std::string& value, Scope scope = Default) const;

    /**
     * @copydoc IStorage::set
     */
    IStorage& set(const std::string& key, const std::string& value, Scope scope = Default);

    /**
     * @copydoc IStorage::remove
     */
    bool remove(const std::string& key, Scope scope = Default);

    /**
     * @copydoc IStorage::clear
     */
    IStorage& clear(Scope scope = Default);

 private:
    typedef Poco::AutoPtr<Poco::Util::PropertyFileConfiguration> StoragePtr;

    static std::string getPath(Scope scope);
    static std::string getFullPath(Scope scope);
    static StoragePtr loadConfig(Scope scope);

    // Default to User scope for now
    UnixStorage() : _defaultScope(User) {}
    UnixStorage(const UnixStorage& o) {}
    ~UnixStorage() {}
    UnixStorage& operator=(const UnixStorage& o) { return *this; }

    StoragePtr getConfig(Scope scope) {
        if (scope == Default) scope = getDefaultScope();
        // default scope must be set, or non-Default value passed in
        assert(scope != Default);

        switch (scope) {
        case Host:
            if (_hostConfig.isNull()) _hostConfig = loadConfig(Host);
            return _hostConfig;
        case User:
            if (_userConfig.isNull()) _userConfig = loadConfig(User);
            return _userConfig;
        default:
            // in case an int is cast to Scope
            return nullptr;
        }
    }

    StoragePtr getConfig(Scope scope) const {
        return const_cast<UnixStorage*>(this)->getConfig(scope);
    }

    void flush(Scope scope);

    mutable Poco::Mutex _mutex;
    Scope _defaultScope;
    StoragePtr _hostConfig;
    StoragePtr _userConfig;
};

}  // namespace BranchIO

#endif  // BRANCHIO_UNIXSTORAGE_H_
