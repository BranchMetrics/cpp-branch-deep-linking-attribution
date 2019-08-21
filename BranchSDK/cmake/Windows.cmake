# On Windows, link against BranchIO.lib for the DLL
# or BranchIOStatic.lib for the static library.
# Must define BRANCHIO_DLL when including headers and building against the DLL (as opposed to the static lib)

# Define when building the DLL on Windows
if (BUILD_SHARED_LIBS)
    set_target_properties(BranchIO PROPERTIES DEFINE_SYMBOL BRANCHIO_BUILD_DLL)
endif()
