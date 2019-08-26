# Visual C++ settings

# Enable C++ exceptions (default in VS projects)
add_compile_options(/EHsc)

# No safe exception handlers
add_link_options(/SAFESEH:NO)

# VS Generator doesn't seem to use CMAKE_CXX_FLAGS, so customize here
if(${CMAKE_BUILD_TYPE} MATCHES Debug)
	add_compile_options(/MDd)
	# Ignore msvcrt.lib
	add_link_options(/NODEFAULTLIB:MSVCRT)
else()
	add_compile_options(/MD)
endif()
