project(ConsoleSamples)

# --------------------
# Example Console Apps
# --------------------

# Console Sample Sources
file(GLOB HELLO_SRC "hello/*.cpp")
file(GLOB EXAMPLE_SRC "example/*.cpp")

add_executable(hello ${HELLO_SRC})
add_executable(example ${EXAMPLE_SRC})

if (BUILD_SHARED_LIBS)
    set_target_properties(hello PROPERTIES DEFINE_SYMBOL BRANCHIO_DLL)
    set_target_properties(example PROPERTIES DEFINE_SYMBOL BRANCHIO_DLL)
endif()

target_link_libraries(hello BranchIO CONAN_PKG::Poco CONAN_PKG::OpenSSL)
target_link_libraries(example BranchIO CONAN_PKG::Poco CONAN_PKG::OpenSSL)
