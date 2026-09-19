cmake_minimum_required(VERSION 3.28)

project(SFMLHarmonyExample LANGUAGES C CXX)

get_filename_component(SFML_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../../.." ABSOLUTE)
if(NOT EXISTS "${SFML_SOURCE_DIR}/include/SFML/Config.hpp")
    message(FATAL_ERROR "Unable to locate the SFML source tree from ${CMAKE_CURRENT_LIST_DIR}")
endif()

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_TEST_SUITE OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(SFML_CONFIGURE_EXTRAS OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_GRAPHICS ON CACHE BOOL "" FORCE)
set(SFML_BUILD_AUDIO ON CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK ON CACHE BOOL "" FORCE)
set(SFML_USE_SYSTEM_DEPS OFF CACHE BOOL "" FORCE)

add_subdirectory("${SFML_SOURCE_DIR}" sfml)

add_library(entry SHARED
    HarmonyExample.cpp
    napi_init.cpp
)

target_compile_features(entry PRIVATE cxx_std_17)
target_compile_options(entry PRIVATE -Wall -Wextra -Wpedantic)
target_include_directories(entry PRIVATE "${SFML_SOURCE_DIR}/src")
target_link_libraries(entry PRIVATE
    SFML::Main
    SFML::Graphics
    SFML::Audio
    SFML::Network
    ace_napi.z
    hilog_ndk.z
)

set_target_properties(entry PROPERTIES OUTPUT_NAME entry)
