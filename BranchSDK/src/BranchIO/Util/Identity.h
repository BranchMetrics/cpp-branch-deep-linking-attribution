#pragma once

#include "BranchIO/String.h"

namespace BranchIO {

struct Identity {
	static String get();
	static void set(const String& identity);
	static void clear();
};

}  // namespace BranchIO

