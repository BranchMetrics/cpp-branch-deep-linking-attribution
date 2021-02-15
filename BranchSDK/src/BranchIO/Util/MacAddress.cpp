// Copyright (c) 2019-21 Branch Metrics, Inc.

#include <exception>

#include "MacAddress.h"

using namespace std;

namespace BranchIO {

MacAddress::MacAddress(const std::string& s) : _address(s) {
}

MacAddress::ComponentList
MacAddress::getComponents() const {
    string::size_type sep(string::npos), prev(0);
    ComponentList components;

    while ((sep = _address.find_first_of(SeparatorChars, prev)) != string::npos) {
        components.push_back(_address.substr(prev, sep-prev));
        prev = sep + 1;
    }
    components.push_back(_address.substr(prev));

    return components;
}

bool
MacAddress::operator==(const MacAddress& o) const {
    ComponentList myComponents(getComponents());
    ComponentList theirComponents(o.getComponents());

    return myComponents == theirComponents;
}

bool
MacAddress::valid() const {
    ComponentList components(getComponents());
    return components.size() == 6;
}

}  // namespace BranchIO
