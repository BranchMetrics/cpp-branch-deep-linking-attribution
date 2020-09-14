#pragma once

#include "BranchIO/String.h"

namespace BranchIO {

class Identity {
 public:
	static Identity& instance();

	String get() const;
	void set(const String& identity);
	void clear();

 private:
    Identity() {}
	~Identity() {}
	Identity(const Identity&) {}
	Identity& operator=(const Identity&) { return *this; }
};

}  // namespace BranchIO

