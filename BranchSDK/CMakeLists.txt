project(BranchIO)

# ------------
# BranchIO SDK
# ------------

# Scan for source code
include_directories(src)
file(GLOB ALL_BRANCH_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/BranchIO/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/BranchIO/**/*.cpp)
set(BRANCH_SOURCE "")

# Scan all file names looking specifically for "Windows" or "Unix", and exclude from the other type of build.
# i.e. exclude Unix files from Windows builds, and vice versa.
# TODO(jdee): Clean this up
if(${CMAKE_SYSTEM_NAME} MATCHES "Win")
    add_definitions(-DWIN32)
    foreach(FILE ${ALL_BRANCH_SOURCE})
        if (NOT ${FILE} MATCHES "Unix")
            set(BRANCH_SOURCE ${BRANCH_SOURCE} ${FILE})
        endif()
    endforeach()
else()
    foreach(FILE ${ALL_BRANCH_SOURCE})
        if (NOT ${FILE} MATCHES "Windows")
            set(BRANCH_SOURCE ${BRANCH_SOURCE} ${FILE})
        endif()
    endforeach()
endif()

# Static/Shared controlled by BUILD_SHARED_LIBS
# BUILD_SHARED_LIBS is passed in through the initial configure step (cmake ../.. -G ...) in rmake[.bat].
# It is enabled by Conan using the shared option (conan install -o BranchIO:shared=true).
# true: BranchIO.lib/BranchIO.dll on Windows, libBranchIO.dylib on Mac, libBranchIO.so on Linux
# false: BranchIO.lib on Windows, libBranchIO.a on Unix
add_library(BranchIO ${BRANCH_SOURCE})

target_include_directories(BranchIO PRIVATE ${CONAN_INCLUDE_DIRS})

# Link the shared lib against Conan deps (ignored when BUILD_SHARED_LIBS=false)
target_link_libraries(BranchIO CONAN_PKG::Poco)

# --------
# Settings
# --------

# Specialize build settings for libraries by platform
include(cmake/${CMAKE_SYSTEM_NAME}.cmake OPTIONAL)

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
