# G++/Clang settings

# Treat all warnings as errors.
add_compile_options(-Wall -Werror)

# Ignore Visual C++ pragmas
add_compile_options(-Wno-unknown-pragmas)

if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
    # Debug:
    # -O0: No optimization
    add_compile_options(-O0)
else()
    # Release:
    # -O3: High optimization (level 3)
    add_compile_options(-O3)
endif()
