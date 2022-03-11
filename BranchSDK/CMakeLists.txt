cmake_minimum_required(VERSION 3.15)

# Enables setting the MSVC runtime library
# https://cmake.org/cmake/help/latest/prop_tgt/MSVC_RUNTIME_LIBRARY.html
cmake_policy(SET CMP0091 NEW)

# Debug or Release settings
# cmake/Debug.cmake, cmake/Release.cmake
include(cmake/${CMAKE_BUILD_TYPE}.cmake OPTIONAL)

project(BranchIO)

# Enable C++ exceptions
add_compile_options(/EHsc)
add_definitions(-DUNICODE -D_UNICODE)

# Make dependencies available via conan
# TODO: TARGETS based version does not work, why?
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
#conan_basic_setup(TARGETS)

# ------------
# BranchIO SDK
# ------------

# BranchIO library
include_directories(src)
file(GLOB BRANCH_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/BranchIO/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/BranchIO/**/*.cpp)
add_library(BranchIO ${BRANCH_SOURCE})

# TODO: TARGETS based version does not work, why?
target_link_libraries(BranchIO ${CONAN_LIBS})

# Set C++ version
target_compile_features(BranchIO PUBLIC cxx_std_17)

# Set MSVC runtime
# TODO: Can this be done via configuration?
if (RUNTIME STREQUAL "MD")
    set_property(TARGET BranchIO PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

    # TODO: release postfix does not work, rename the binary later
    set_target_properties(BranchIO PROPERTIES RELEASE_POSTFIX "md")
endif()

if (RUNTIME STREQUAL "MDd")
    set_property(TARGET BranchIO PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL")
    set_target_properties(BranchIO PROPERTIES DEBUG_POSTFIX "mdd")
endif()

if (RUNTIME STREQUAL "MT")
    set_property(TARGET BranchIO PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")

    # TODO: release postfix does not work, rename the binary later
    set_target_properties(BranchIO PROPERTIES RELEASE_POSTFIX "mt")
endif()

if (RUNTIME STREQUAL "MTd")
    set_property(TARGET BranchIO PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
    set_target_properties(BranchIO PROPERTIES DEBUG_POSTFIX "mtd")
endif()

# TODO: support DLLs. Check if LD and LDd are needed
#if (BUILD_SHARED_LIBS)
#    set_target_properties(BranchIO PROPERTIES DEFINE_SYMBOL BRANCHIO_BUILD_DLL)
#endif()

# ----------
# Unit tests
# ----------

# TODO: fix the unit test setup

# Include test code
#file(GLOB TEST ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp)

# When building a DLL on Windows, there are visibility issues. We'll always
# have the need to unit-test internal stuff that we don't want to make
# public with BRANCHIO_DLL_EXPORT. So we can't link unit tests against the
# DLL. We could always build static, at least on Windows, even when building
# shared libraries, but for now it seems best to build against source.
#add_executable(unit_tests ${TEST} ${BRANCH_SOURCE})
#target_link_libraries(unit_tests CONAN_PKG::gtest)

# Generates JUnit-style output in test_detail.xml, e.g. build/Debug/test_detail.xml.
#add_test(NAME UnitTests COMMAND unit_tests --gtest_output=xml)
