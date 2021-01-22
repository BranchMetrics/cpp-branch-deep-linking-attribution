// Copyright (c) 2019-21 Branch Metrics, Inc.

#include "BranchIO/Event/SessionEvent.h"
#include "BranchIO/Util/Log.h"

#include <Poco/URI.h>

#include <sstream>

using namespace std;
using namespace Poco;

namespace BranchIO {

BaseEvent& SessionOpenEvent::setLinkUrl(const String &url) {
    string surl(url.str());

    // The link_identifier is sent in JSON as a number, equal
    // to the link_click_id query parameter from the inbound
    // URI (url argument here).
    try {
        URI uri(surl); // throws SyntaxException when invalid

        // vector<pair<string, string>>
        // ordered, not associative container
	auto queryParams = uri.getQueryParameters();
	for (URI::QueryParameters::const_iterator it=queryParams.begin(); it != queryParams.end(); ++it) {
            if (it->first == "link_click_id") {
                unsigned int linkClickId(0);
                istringstream iss(it->second);
                iss >> linkClickId;

		// Defines::JSONKEY_APP_LINK_URL is "link_identifier". See Defines.cpp.
                addEventProperty(Defines::JSONKEY_APP_LINK_URL, linkClickId);
                break;
            }
        }
    } catch (Exception& e) {
        // Poco exceptions thrown by URI constructor
        BRANCH_LOG_E("Invalid URI: \"" << surl << "\" (" << e.message() << ")");
    } catch (runtime_error& e) {
        // STL exceptions thrown by operator >> in case of non-numeric link_click_id
        BRANCH_LOG_E("Invalid link_click_id: \"" << surl << "\" (" << e.what() << ")");
    }

    return *this;
}

}  // namespace BranchIO
