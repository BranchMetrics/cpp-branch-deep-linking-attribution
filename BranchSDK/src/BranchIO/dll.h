// Copyright (c) 2019 Branch Metrics, Inc.

#ifndef BRANCHIO_DLL_H__
#define BRANCHIO_DLL_H__

#ifdef WIN32
    #ifdef BRANCHIO_BUILD_DLL
        #define BRANCHIO_DLL_EXPORT __declspec(dllexport)
    #endif  // BRANCHIO_BUILD_DLL
    #ifdef BRANCHIO_DLL
        #define BRANCHIO_DLL_EXPORT __declspec(dllimport)
    #endif  // BRANCHIO_DLL
#endif  // BRANCHIO_DLL

#ifndef BRANCHIO_DLL_EXPORT
#define BRANCHIO_DLL_EXPORT
#endif  // BRANCHIO_DLL_EXPORT

#endif  // BRANCHIO_DLL_H__
