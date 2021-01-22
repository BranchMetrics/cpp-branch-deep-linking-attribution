// Copyright (c) 2019 Branch Metrics, Inc.

#include "SessionEvent.h"

namespace BranchIO {
    BaseEvent& SessionOpenEvent::setLinkUrl(const String &url) {
        if (url.str().length() > 0) {
            addEventProperty(Defines::JSONKEY_APP_LINK_URL, url);
        }
        return *this;
    }

}  // namespace BranchIO
