cmake_minimum_required(VERSION 3.15)

project(root VERSION 2.1.0 LANGUAGES CXX)

# Enable Unit Testing.
# TODO: Investigate why this appears to be needed at the top level CMakeLists
enable_testing()

# ------------
# BranchIO SDK
# ------------
add_subdirectory(BranchSDK)
include_directories("BranchSDK/src")
