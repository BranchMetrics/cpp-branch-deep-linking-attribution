cmake_minimum_required(VERSION 3.15)

# Enables setting the MSVC runtime library
# https://cmake.org/cmake/help/latest/prop_tgt/MSVC_RUNTIME_LIBRARY.html
cmake_policy(SET CMP0091 NEW)

project(BranchIO)

# ------------
# Visual C++ settings
# ------------

# This should be automatically set by cmake 
# https://cmake.org/cmake/help/latest/variable/WIN32.html
#add_definitions(-DWIN32)

# Enable C++ exceptions
add_compile_options(/EHsc)

# TODO: confirm this is not necessary
# Disables protection against Structured Exception Handler (SEH) buffer overflows in 32 bit apps
#add_link_options(/SAFESEH:NO)

# old way of setting runtime
#add_compile_options(/MDd)
#add_link_options(/NODEFAULTLIB:MSVCRT)

# ------------
# BranchIO SDK
# ------------

# Set dependencies via Conan
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# BranchIO library
include_directories(src)
file(GLOB BRANCH_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/BranchIO/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/BranchIO/**/*.cpp)
add_library(BranchIO ${BRANCH_SOURCE})

#find_package(poco)
target_link_libraries(BranchIO ${CONAN_LIBS})

# Set C++ version
target_compile_features(BranchIO PUBLIC cxx_std_11)

# Set MSVC runtime
set_property(TARGET BranchIO PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")

#target_include_directories(BranchIO PRIVATE ${CONAN_INCLUDE_DIRS})

# Link the shared lib against Conan deps (ignored when BUILD_SHARED_LIBS=false)
#find_package(Poco REQUIRED)
#target_link_libraries(BranchIO CONAN_PKG::Poco)

# ------------
# Installation
# ------------

if(NOT SKIP_INSTALL_ALL)
    message(STATUS "Installing to ${CMAKE_INSTALL_PREFIX}")

    # https://cmake.org/cmake/help/v3.14/command/install.html
    include(GNUInstallDirs)
    install(TARGETS BranchIO)

    # TODO(jdee): Refine public/internal header distinction. For now, recursively install everything as public.
    install(DIRECTORY BranchIO DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/BranchIO FILES_MATCHING PATTERN "*.h")
endif()

# ----------
# Unit tests
# ----------

# Include test code
#file(GLOB TEST ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp)

# When building a DLL on Windows, there are visibility issues. We'll always
# have the need to unit-test internal stuff that we don't want to make
# public with BRANCHIO_DLL_EXPORT. So we can't link unit tests against the
# DLL. We could always build static, at least on Windows, even when building
# shared libraries, but for now it seems best to build against source.
#add_executable(unit_tests ${TEST} ${BRANCH_SOURCE})
#target_link_libraries(unit_tests CONAN_PKG::Poco CONAN_PKG::gtest)

# Generates JUnit-style output in test_detail.xml, e.g. build/Debug/test_detail.xml.
#add_test(NAME UnitTests COMMAND unit_tests --gtest_output=xml)
