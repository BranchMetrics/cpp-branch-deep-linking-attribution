#include "BranchIO/Util/Identity.h"
#include "BranchIO/SessionInfo.h"
#include "BranchIO/Util/Storage.h"

using namespace std;

namespace BranchIO {

Identity&
Identity::instance() {
	static Identity _instance;
	return _instance;
}

String
Identity::get() const {
	return Storage::instance().getString(string(SESSIONSTORAGE) + ".identity");
}

void
Identity::set(const String& identity) {
	Storage::instance().setString(string(SESSIONSTORAGE) + ".identity", identity.str());
}

}  // namespace BranchIO