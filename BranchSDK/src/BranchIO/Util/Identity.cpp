#include "BranchIO/Util/Identity.h"
#include "BranchIO/Util/Storage.h"

using namespace std;

namespace BranchIO {

static constexpr const char* const StorageKey = "session.identity";

String
Identity::get() {
	return Storage::instance().getString(StorageKey);
}

void
Identity::set(const String& identity) {
	Storage::instance().setString(StorageKey, identity.str());
}

void
Identity::clear() {
	Storage::instance().remove(StorageKey);
}

}  // namespace BranchIO