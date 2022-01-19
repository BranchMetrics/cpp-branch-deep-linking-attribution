// Copyright (c) 2019-21 Branch Metrics, Inc.

#include "BranchIO/Event/SessionEvent.h"
#include "BranchIO/Util/Log.h"
#include "BranchIO/Util/StringUtils.h"
#include <winrt/Windows.Foundation.h>


#include <sstream>

using namespace std;
using namespace winrt;
using namespace winrt::Windows::Foundation;

namespace BranchIO {

BaseEvent& SessionOpenEvent::setLinkUrl(const String &url) {
    string surl(url.str());

    // The link_identifier is sent in JSON as a number, equal
    // to the link_click_id query parameter from the inbound
    // URI (url argument here).
    try {
        Uri uri(to_hstring(surl)); // throws SyntaxException when invalid

        WwwFormUrlDecoder queryParams = uri.QueryParsed();
        string linkClickId = to_string( queryParams.GetFirstValueByName(L"link_click_id"));
        addEventProperty(Defines::JSONKEY_LINK_IDENTIFIER, linkClickId);
            
    } 
    catch (winrt::hresult_error const& ex){
            hresult hr = ex.code(); // HRESULT_FROM_WIN32.
            string message = to_string(ex.message()); 
            BRANCH_LOG_E("Invalid URI: \"" << surl << "\" (" << hr << " - " << message << ")");
        }
    catch (runtime_error& e) {
        // STL exceptions thrown by operator >> in case of non-numeric link_click_id
        BRANCH_LOG_E("Invalid link_click_id: \"" << surl << "\" (" << e.what() << ")");
    }
    return *this;
}

}  // namespace BranchIO
