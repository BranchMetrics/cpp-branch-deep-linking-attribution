# Visual C++ settings

# Enable C++ exceptions (default in VS projects)
add_compile_options(/EHsc)

# No safe exception handlers
add_link_options(/SAFESEH:NO)

# Ignore msvcrt.lib and friends.w
add_link_options(/NODEFAULTLIB:MSVCRT)
