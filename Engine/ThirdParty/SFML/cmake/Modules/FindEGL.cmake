#
# Try to find EGL library and include path.
# Once done this will define
#
# EGL_FOUND
# EGL_INCLUDE_DIR
# EGL_LIBRARY
#

find_path(EGL_INCLUDE_DIR EGL/egl.h PATHS ${FIND_SFML_PATHS} PATH_SUFFIXES include)
find_library(EGL_LIBRARY NAMES EGL libEGL PATHS ${FIND_SFML_PATHS} PATH_SUFFIXES lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(EGL DEFAULT_MSG EGL_LIBRARY EGL_INCLUDE_DIR)

if(EGL_FOUND AND NOT TARGET EGL::EGL)
    add_library(EGL::EGL IMPORTED UNKNOWN)
    set_target_properties(EGL::EGL PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${EGL_INCLUDE_DIR}"
        IMPORTED_LOCATION "${EGL_LIBRARY}")
endif()
