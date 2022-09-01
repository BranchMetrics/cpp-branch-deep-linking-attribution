#include "ConsoleLogChannel.h"
#include "StringUtils.h"

using namespace std;

namespace BranchIO
{
    void ConsoleLogChannel::log(const std::string& message)
    {
        scoped_lock _l(_mutex);
        OutputDebugString(StringUtils::utf8_to_wstring(message).c_str());
        OutputDebugString(L"\n");
    }
}
