cmake_minimum_required(VERSION 3.12)

project(root VERSION 1.2.1 LANGUAGES CXX)

# Determines handling of RPATH and related variables when building dylibs on Mac.
# TODO(jdee): Review this. RPATH is an issue on Unix right now.
cmake_policy(SET CMP0068 NEW)

SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

# ------------
#   Settings
# ------------

# Specialize compiler flags by build type
# cmake/Debug.cmake, cmake/Release.cmake
include(cmake/${CMAKE_BUILD_TYPE}.cmake OPTIONAL)

# Specialize compiler flags by platform
include(cmake/${CMAKE_SYSTEM_NAME}.cmake OPTIONAL)

# -----------
# Conan setup
# -----------
if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

    # conan_basic_setup(TARGETS): Create CONAN_PKG::Poco, etc. targets to link
    # against. This is useful to keep gtest from being linked in as a runtime
    # dependency of our library and examples.
    # https://docs.conan.io/en/latest/integrations/build_system/cmake/cmake_generator.html#targets-approach
    conan_basic_setup(TARGETS)
else()
    message(WARNING "The file conanbuildinfo.cmake doesn't exist, you have to run conan install first")
endif()

# Enable Unit Testing.
# TODO(andyp): Investigate why this appears to be needed at the top level CMakeLists
enable_testing()

# ------------
# BranchIO SDK
# ------------
ADD_SUBDIRECTORY(BranchSDK)
include_directories("BranchSDK/src")
