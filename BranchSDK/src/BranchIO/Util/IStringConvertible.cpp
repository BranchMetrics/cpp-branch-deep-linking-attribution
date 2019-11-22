// Copyright (c) 2019 Branch Metrics, Inc.

#include <iostream>

#include "IStringConvertible.h"

std::ostream& operator<<(
    std::ostream& s,
    BranchIO::IStringConvertible const& item) {

    return s << item.toString();
}
