// Copyright (c) 2019 Branch Metrics, Inc.

#ifndef BRANCHIO_UTIL_ISTORAGE_H__
#define BRANCHIO_UTIL_ISTORAGE_H__

#include <memory>
#include <string>

namespace BranchIO {

/**
 * Storage interface
 */
struct IStorage {
    virtual ~IStorage() {}

    /**
     * Scope enumeration
     */
    enum Scope {
        /// Indicates unset scope
        Default,
        /// Indicates host-wide setting
        Host,
        /// Indicates per-user setting
        User
    };

    /**
     * Get the default scope for storage
     * @return the current default scope
     */
    virtual Scope getDefaultScope() const = 0;

    /**
     * Set the default scope
     * @param scope new scope to use as default
     * @return *this
     */
    virtual IStorage& setDefaultScope(Scope scope) = 0;

    virtual std::string getPrefix() const = 0;
    virtual IStorage& setPrefix(const std::string& prefix) = 0;

    /**
     * Determine if the specified key is present for the specified scope
     * (or default scope)
     * @param key a key to check
     * @param scope the scope for this key (optional if default scope set)
     * @return true if set, false otherwise
     */
    virtual bool has(const std::string& key, Scope scope = Default) const = 0;

    /**
     * Get the current value of a key.  If not found, return the defaultValue.
     * @param key a key to retrieve
     * @param defaultValue a string to use if the key is not found
     * @param scope the scope for this key (optional if default scope set)
     * @return the value of the key if found, the defaultValue otherwise
     */
    virtual std::string getString(const std::string& key, const std::string& defaultValue = "", Scope scope = Default) const = 0;

    /**
     * Set a new value for the specified key and scope (or default scope)
     * @param key a key to set
     * @param value a new value for the specified key
     * @param scope the scope for this key (optional if default scope set)
     * @return *this
     */
    virtual IStorage& setString(const std::string& key, const std::string& value, Scope scope = Default) = 0;

    /**
     * Get the current value of a key.  If not found, return the defaultValue.
     * @param key a key to retrieve
     * @param defaultValue a bool to use if the key is not found
     * @param scope the scope for this key (optional if default scope set)
     * @return the value of the key if found, the defaultValue otherwise
     */
    virtual bool getBoolean(const std::string& key, bool defaultValue = false, Scope scope = Default) const = 0;

    /**
     * Set a new value for the specified key and scope (or default scope)
     * @param key a key to set
     * @param value a new value for the specified key
     * @param scope the scope for this key (optional if default scope set)
     * @return *this
     */
    virtual IStorage& setBoolean(const std::string& key, bool value, Scope scope = Default) = 0;

    /**
     * Remove a key and its corresponding value (if any) for the specified scope or default scope
     * @param key a key to remove
     * @param scope the scope for this key (optional if default scope set)
     * @return true if the key was removed, false if not found
     */
    virtual bool remove(const std::string& key, Scope scope = Default) = 0;

    /**
     * Remove all entries (removes the entire Windows Registry BranchIO key as well)
     * @param scope the scope to clear
     * @return *this
     */
    virtual IStorage& clear(Scope scope = Default) = 0;
};

}  // namespace BranchIO

#endif  // BRANCHIO_UTIL_ISTORAGE_H__
