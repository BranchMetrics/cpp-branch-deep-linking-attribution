#include <BranchIO/Log.h>
#include <BranchIO/Storage.h>
#include <Poco/Exception.h>
#include <exception>

using namespace std;
using namespace BranchIO;

int
main(int argc, char **argv) {
    try {
        Storage& storage(Storage::instance());
        storage.setDefaultScope(Storage::User);

        const string keyName("session.data");

        storage.set(keyName, "xyz");
        string value;
        if (!storage.get(keyName, value)) {
            BRANCH_LOG_E("Value not found");
            return 1;
        }

        BRANCH_LOG_I("value for key " << keyName << " is \"" << value << "\"");

        storage.remove(keyName);

        if (storage.has(keyName)) {
            BRANCH_LOG_E("Key not removed");
            return 1;
        }

        BRANCH_LOG_I("key " << keyName << " successfully removed");

        return 0;
    }
    catch (const Poco::Exception& e) {
        BRANCH_LOG_E("Exception in main (" << e.what() << "): " << e.message());
    }
    catch (const std::exception& e) {
        BRANCH_LOG_E("Exception in main: " << e.what());
    }
    catch (...) {
        BRANCH_LOG_E("Unexpected exception in main");
    }
    return -1;
}
