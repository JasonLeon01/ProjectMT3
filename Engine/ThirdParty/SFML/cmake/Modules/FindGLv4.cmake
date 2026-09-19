#
# Try to find HarmonyOS desktop OpenGL headers and libGLv4.
# Once done this will define
#
# GLv4_FOUND
# GLv4_INCLUDE_DIR
# GLv4_LIBRARY
# GLv4::GLv4
#

set(_SFML_GLV4_INCLUDE_HINTS)
if(CMAKE_SYSTEM_NAME STREQUAL "OHOS" AND CMAKE_SYSROOT)
    list(APPEND _SFML_GLV4_INCLUDE_HINTS "${CMAKE_SYSROOT}/usr/include")
endif()

find_path(GLv4_INCLUDE_DIR GL/gl.h
          HINTS ${_SFML_GLV4_INCLUDE_HINTS}
          PATHS ${FIND_SFML_PATHS}
          PATH_SUFFIXES include)
find_library(GLv4_LIBRARY NAMES GLv4 libGLv4 PATHS ${FIND_SFML_PATHS} PATH_SUFFIXES lib)

unset(_SFML_GLV4_INCLUDE_HINTS)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLv4 DEFAULT_MSG GLv4_LIBRARY GLv4_INCLUDE_DIR)

if(GLv4_FOUND AND NOT TARGET GLv4::GLv4)
    add_library(GLv4::GLv4 IMPORTED UNKNOWN)
    set_target_properties(GLv4::GLv4 PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${GLv4_INCLUDE_DIR}"
        IMPORTED_LOCATION "${GLv4_LIBRARY}")
endif()

mark_as_advanced(GLv4_INCLUDE_DIR GLv4_LIBRARY)
