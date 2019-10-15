// Copyright (c) 2019 Branch Metrics, Inc.

#ifndef BRANCHIO_MACADDRESS_H__
#define BRANCHIO_MACADDRESS_H__

#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "IStringConvertible.h"

namespace BranchIO {

/**
 * MacAddress abstraction
 */
class MacAddress : public virtual IStringConvertible {
 public:
    /**
     * Characters recognized as separators in MAC addresses
     */
    static constexpr const char *const SeparatorChars = ":-";

    /**
     * Return type of getComponents()
     */
    typedef std::vector<std::string> ComponentList;

    /**
     * Conversion operator for vector types
     * @tparam T the type contained by the argument
     * @param v a vector
     */
    template <class T>
    MacAddress(const std::vector<T>& v) {  // NOLINT
        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        for (typename std::vector<T>::const_iterator it = v.begin(); it != v.end(); ++it) {
            oss << std::setw(2) << int(*it);
            oss << std::setw(1) << ":";
        }
        std::string macString(oss.str());

        _address = macString.substr(0, macString.length() - 1);
    }

    /**
     * Conversion operator and default constructor
     * @param s (optional) a string separated by characters in SeparatorChars (default: blank)
     */
    MacAddress(const std::string& s = std::string());  // NOLINT

    /**
     * Returns a list of components separated by SeparatorChars
     * @return a vector of strings containing components
     */
    ComponentList getComponents() const;

    /**
     * Compare with another MacAddress object
     * @param o another MacAddress object
     * @return true if the two are semantically equal, false otherwise
     */
    bool operator==(const MacAddress& o) const;

    /**
     * Compare with another MacAddress object
     * @param o another MacAddress object
     * @return true if the two are semantically unequal, false otherwise
     */
    bool operator!=(const MacAddress& o) const {
        return !(*this == o);
    }

    /**
     * Determine if this object holds a MacAddress
     * @return true if empty, false otherwise
     */
    bool empty() const {
        return _address.empty();
    }

    /**
     * Determine whether this is a valid MAC address
     * @return true if valid, false otherwise
     */
    bool valid() const;

    /**
     * String conversion
     * @return the underlying MacAddress as a string
     */
    std::string toString() const { return _address; }

 private:
    std::string _address;
};

}  // namespace BranchIO

#endif  // BRANCHIO_MACADDRESS_H__
