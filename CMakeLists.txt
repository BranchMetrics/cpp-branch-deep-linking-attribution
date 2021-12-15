cmake_minimum_required(VERSION 3.15)

project(root VERSION 1.2.3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

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
#enable_testing()

# ------------
# BranchIO SDK
# ------------
add_subdirectory(BranchSDK)
include_directories("BranchSDK/src")
