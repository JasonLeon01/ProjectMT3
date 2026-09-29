# OpenHarmony/HarmonyOS build contract.
#
# The official DevEco toolchain owns the compiler, sysroot, ABI and C++ runtime
# selection. This module validates that contract and configures SFML-specific
# policy after GNUInstallDirs has initialized the install directory variables.

include_guard(DIRECTORY)

set(_SFML_HARMONY_MIN_SDK_API 22)

sfml_set_option(SFML_HARMONY_DEVICE_FORM MOBILE STRING "OpenHarmony device form (MOBILE or 2IN1)")
string(TOUPPER "${SFML_HARMONY_DEVICE_FORM}" SFML_HARMONY_DEVICE_FORM)
set(SFML_HARMONY_DEVICE_FORM "${SFML_HARMONY_DEVICE_FORM}" CACHE STRING
    "OpenHarmony device form (MOBILE or 2IN1)" FORCE)
set_property(CACHE SFML_HARMONY_DEVICE_FORM PROPERTY STRINGS MOBILE 2IN1)

if(SFML_HARMONY_DEVICE_FORM STREQUAL "MOBILE")
    # Config.cmake supplies the generic Harmony default. Keep the device-form
    # policy here so the root option can distinguish MOBILE from 2IN1.
    set(OPENGL_ES ON)
elseif(SFML_HARMONY_DEVICE_FORM STREQUAL "2IN1")
    set(OPENGL_ES OFF)
else()
    message(FATAL_ERROR "Unsupported SFML_HARMONY_DEVICE_FORM: ${SFML_HARMONY_DEVICE_FORM}")
endif()

# The form controls public compile definitions, system libraries and the
# default GL client API. Reusing one cache for another form can retain a stale
# forced MOBILE GLES value and silently produce the wrong ABI. Require a fresh
# or form-specific build directory instead of guessing whether the cached
# value was set explicitly by the user.
if(DEFINED _SFML_HARMONY_CONFIGURED_DEVICE_FORM AND
   NOT "${_SFML_HARMONY_CONFIGURED_DEVICE_FORM}" STREQUAL "${SFML_HARMONY_DEVICE_FORM}")
    message(FATAL_ERROR
        "SFML_HARMONY_DEVICE_FORM changed from ${_SFML_HARMONY_CONFIGURED_DEVICE_FORM} to "
        "${SFML_HARMONY_DEVICE_FORM}; use cmake --fresh or a separate build directory for each Harmony device form")
endif()
set(_SFML_HARMONY_CONFIGURED_DEVICE_FORM "${SFML_HARMONY_DEVICE_FORM}" CACHE INTERNAL
    "Harmony device form used to initialize this build directory" FORCE)

if(NOT DEFINED OHOS_SDK_NATIVE OR NOT EXISTS "${OHOS_SDK_NATIVE}/oh-uni-package.json")
    message(FATAL_ERROR "The OpenHarmony native SDK metadata could not be found; use the DevEco OHOS CMake toolchain")
endif()

file(READ "${OHOS_SDK_NATIVE}/oh-uni-package.json" _SFML_HARMONY_SDK_METADATA)
string(JSON _SFML_HARMONY_SDK_API ERROR_VARIABLE _SFML_HARMONY_SDK_API_ERROR
       GET "${_SFML_HARMONY_SDK_METADATA}" apiVersion)
if(_SFML_HARMONY_SDK_API_ERROR OR _SFML_HARMONY_SDK_API LESS _SFML_HARMONY_MIN_SDK_API)
    message(FATAL_ERROR
        "SFML's OpenHarmony ${SFML_HARMONY_DEVICE_FORM} backend requires a native SDK with API level "
        "${_SFML_HARMONY_MIN_SDK_API} or newer")
endif()

# The DevEco toolchain encodes compatibleSdkVersion in its compiler target.
# An API 24 compile SDK can therefore still produce an older native binary
# that bypasses SFML's API 22 availability contract for either device form.
if(NOT DEFINED OHOS_COMPATIBLE_SDK_VERSION OR OHOS_COMPATIBLE_SDK_VERSION STREQUAL "")
    message(FATAL_ERROR
        "SFML's OpenHarmony ${SFML_HARMONY_DEVICE_FORM} backend requires an explicit "
        "OHOS_COMPATIBLE_SDK_VERSION of API level 22 or newer")
endif()

if(OHOS_COMPATIBLE_SDK_VERSION MATCHES "\\(([0-9]+)\\)")
    set(_SFML_HARMONY_COMPATIBLE_SDK_API "${CMAKE_MATCH_1}")
elseif(OHOS_COMPATIBLE_SDK_VERSION MATCHES "^([0-9]+)(\\.|$)")
    set(_SFML_HARMONY_COMPATIBLE_SDK_API "${CMAKE_MATCH_1}")
else()
    message(FATAL_ERROR
        "Unable to determine the API level from OHOS_COMPATIBLE_SDK_VERSION="
        "${OHOS_COMPATIBLE_SDK_VERSION}")
endif()

if(_SFML_HARMONY_COMPATIBLE_SDK_API LESS _SFML_HARMONY_MIN_SDK_API)
    message(FATAL_ERROR
        "SFML's OpenHarmony ${SFML_HARMONY_DEVICE_FORM} backend requires OHOS_COMPATIBLE_SDK_VERSION API level "
        "${_SFML_HARMONY_MIN_SDK_API} or newer")
endif()

if(NOT DEFINED OHOS_STL OR NOT OHOS_STL STREQUAL "c++_shared")
    message(FATAL_ERROR "SFML's OpenHarmony backend requires the DevEco toolchain with OHOS_STL=c++_shared")
endif()

# OHOS_ARCH is selected and validated by the official toolchain. SFML only
# requires its value to keep independently installed ABI artifacts separate.
if(NOT DEFINED OHOS_ARCH OR OHOS_ARCH STREQUAL "")
    message(FATAL_ERROR "The DevEco OHOS CMake toolchain did not provide OHOS_ARCH")
endif()
if(NOT CMAKE_INSTALL_LIBDIR MATCHES "(^|/)${OHOS_ARCH}/?$")
    set(CMAKE_INSTALL_LIBDIR "${CMAKE_INSTALL_LIBDIR}/${OHOS_ARCH}")
endif()
# GNUInstallDirs computed the absolute companion before this module appended
# the ABI. Keep optional pkg-config and other consumers on the same ABI path.
GNUInstallDirs_get_absolute_install_dir(CMAKE_INSTALL_FULL_LIBDIR CMAKE_INSTALL_LIBDIR LIBDIR)

# Static SFML and bundled dependency archives are normally folded into one HAP
# NAPI shared object. Defining the default here, before any SFML or dependency
# targets are created, gives this module sole ownership of Harmony PIC policy.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(SFML_HARMONY_DEVICE_FORM STREQUAL "MOBILE")
    set(_SFML_HARMONY_DEFAULT_GRAPHICS_API "OpenGL ES")
else()
    set(_SFML_HARMONY_DEFAULT_GRAPHICS_API "desktop OpenGL")
endif()

message(STATUS
    "SFML OpenHarmony ${SFML_HARMONY_DEVICE_FORM}: minimum SDK API ${_SFML_HARMONY_MIN_SDK_API}, "
    "ABI ${OHOS_ARCH}, SDK API ${_SFML_HARMONY_SDK_API}, default graphics API ${_SFML_HARMONY_DEFAULT_GRAPHICS_API}")
