// Copyright (c) 2019-21 Branch Metrics, Inc.

#ifndef BRANCHIO_UTIL_ISTRINGCONVERTIBLE_H__
#define BRANCHIO_UTIL_ISTRINGCONVERTIBLE_H__

#include <iosfwd>
#include <string>

#include "BranchIO/dll.h"

namespace BranchIO {

/**
 * (Internal) Utility for converting classes to strings.
 */
struct BRANCHIO_DLL_EXPORT IStringConvertible {
    virtual ~IStringConvertible() {}

    /**
     * @return a string representation of this class.
     */
    virtual std::string toString() const = 0;
};

}  // namespace BranchIO

BRANCHIO_DLL_EXPORT std::ostream& operator<<(
    std::ostream& s,
    BranchIO::IStringConvertible const& item);

#endif  // BRANCHIO_UTIL_ISTRINGCONVERTIBLE_H__
