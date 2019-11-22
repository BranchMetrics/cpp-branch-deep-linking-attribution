// Copyright (c) 2019 Branch Metrics, Inc.

#ifndef BRANCHIO_UTIL_STORAGE_H__
#define BRANCHIO_UTIL_STORAGE_H__

#ifdef WIN32
#include "WindowsStorage.h"
#else
#include "UnixStorage.h"
#endif  // WIN32

namespace BranchIO {
#ifdef WIN32
typedef WindowsStorage Storage;
#else
typedef UnixStorage Storage;
#endif  // WIN32
}  // namespace BranchIO

#endif  // BRANCHIO_UTIL_STORAGE_H__
