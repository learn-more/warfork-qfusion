# - Find openal
# Find the native openal includes and libraries
#
#  OPENAL_INCLUDE_DIR - where to find al.h, etc.
#  OPENAL_LIBRARIES   - List of libraries when using openal.
#  OPENAL_FOUND       - True if openal found.

if(OPENAL_INCLUDE_DIR)
    # Already in cache, be silent
    set(OPENAL_FIND_QUIETLY TRUE)
endif(OPENAL_INCLUDE_DIR)
find_path(OPENAL_INCLUDE_DIR al.h PATH_SUFFIXES include)
# MSVC built openal may be named openal_static.
# The provided project files name the library with the lib prefix.
find_library(OPENAL_LIBRARY NAMES openal openal_static libopenal libopenal_static soft_oal OpenAL32 oal al)
# Handle the QUIETLY and REQUIRED arguments and set OPENAL_FOUND
# to TRUE if all listed variables are TRUE.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OpenAL DEFAULT_MSG OPENAL_INCLUDE_DIR OPENAL_LIBRARY)

set(OPENAL_LIBRARIES ${OPENAL_LIBRARY})

mark_as_advanced(OPENAL_INCLUDE_DIR)
mark_as_advanced(OPENAL_LIBRARY)