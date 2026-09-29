#
# Try to find EGL library and include path.
# Once done this will define
#
# EGL_FOUND
# EGL_INCLUDE_PATH
# EGL_LIBRARY
#

set(_SFML_EGL_INCLUDE_HINTS)
if(CMAKE_SYSTEM_NAME STREQUAL "OHOS" AND CMAKE_SYSROOT)
    list(APPEND _SFML_EGL_INCLUDE_HINTS "${CMAKE_SYSROOT}/usr/include")
endif()

find_path(EGL_INCLUDE_DIR EGL/egl.h
          HINTS ${_SFML_EGL_INCLUDE_HINTS}
          PATHS ${FIND_SFML_PATHS}
          PATH_SUFFIXES include)
find_library(EGL_LIBRARY NAMES EGL libEGL PATHS ${FIND_SFML_PATHS} PATH_SUFFIXES lib)

unset(_SFML_EGL_INCLUDE_HINTS)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(EGL DEFAULT_MSG EGL_LIBRARY EGL_INCLUDE_DIR)

if(EGL_FOUND AND NOT TARGET EGL::EGL)
    add_library(EGL::EGL IMPORTED UNKNOWN)
    set_target_properties(EGL::EGL PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${EGL_INCLUDE_DIR}"
        IMPORTED_LOCATION "${EGL_LIBRARY}")
endif()
