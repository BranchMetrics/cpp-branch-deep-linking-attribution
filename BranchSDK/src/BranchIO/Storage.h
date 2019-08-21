// Copyright (c) 2019 Branch Metrics, Inc.

#ifndef BRANCHIO_STORAGE_H__
#define BRANCHIO_STORAGE_H__

#ifdef WIN32
#include "BranchIO/WindowsStorage.h"
#else
#include "BranchIO/UnixStorage.h"
#endif  // WIN32

namespace BranchIO {
#ifdef WIN32
typedef WindowsStorage Storage;
#else
typedef UnixStorage Storage;
#endif  // WIN32
}  // namespace BranchIO

#endif  // BRANCHIO_STORAGE_H__
