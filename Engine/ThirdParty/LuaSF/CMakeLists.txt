cmake_minimum_required(VERSION 3.18...3.31)

project("LuaSF" LANGUAGES C CXX)

set(LUASF_ROOT "${CMAKE_CURRENT_LIST_DIR}")
set(LUASF_BINARY_ROOT "${CMAKE_CURRENT_BINARY_DIR}")
get_filename_component(_LUASF_CALLBACK_CODECS_FILE "${LUASF_ROOT}/callback_codecs.json" ABSOLUTE)
set(LUASF_CALLBACK_CODECS_FILE "${_LUASF_CALLBACK_CODECS_FILE}" CACHE FILEPATH "LuaSF callback codec manifest" FORCE)
unset(_LUASF_CALLBACK_CODECS_FILE)
if(NOT EXISTS "${LUASF_CALLBACK_CODECS_FILE}")
    message(FATAL_ERROR "LuaSF callback codec manifest was not found: ${LUASF_CALLBACK_CODECS_FILE}")
endif()
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS OFF)
set(LUASF_RELEASE_LTO ON)
if(MINGW)
    set(LUASF_RELEASE_LTO OFF)
endif()
set(LUASF_EMBEDDED_OUTPUT_DIR "${LUASF_BINARY_ROOT}/bin/$<CONFIG>/embedded")
set(LUASF_EMBEDDED_ARCHIVE_DIR "${LUASF_BINARY_ROOT}/lib/$<CONFIG>/embedded")

set(LUASF_SFML_ROOT "" CACHE PATH "External SFML source project used by LuaSF")
set(LUASF_LUA_ROOT "" CACHE PATH "External Lua source directory containing src/lua.h")
set(LUASF_GLUE_ROOT "${LUASF_ROOT}/../LuaGlue" CACHE PATH "LuaGlue source project")
if(NOT LUASF_SFML_ROOT)
    message(FATAL_ERROR "LUASF_SFML_ROOT is required; point it at an SFML source project.")
endif()
if(NOT EXISTS "${LUASF_SFML_ROOT}/CMakeLists.txt")
    message(FATAL_ERROR "SFML source project was not found: ${LUASF_SFML_ROOT}")
endif()
if(NOT EXISTS "${LUASF_LUA_ROOT}/src/lua.h")
    message(FATAL_ERROR "Lua source directory was not found: ${LUASF_LUA_ROOT}")
endif()

option(LUASF_BUILD_SHARED_SFML "Build SFML as shared libraries for LuaSF" ON)
option(LUASF_GENERATE_LUA_STUB "Generate Lua language-server stub from the built LuaSF module" ON)
if(CMAKE_SYSTEM_NAME STREQUAL "iOS"
   OR CMAKE_SYSTEM_NAME STREQUAL "Android"
   OR CMAKE_SYSTEM_NAME STREQUAL "OHOS")
    set(LUASF_BUILD_SHARED_SFML OFF CACHE BOOL "Build SFML as shared libraries for LuaSF" FORCE)
    set(LUASF_LIBRARY_TYPE STATIC)
else()
    set(LUASF_LIBRARY_TYPE SHARED)
endif()
if(NOT DEFINED LUASF_LUA_STUB_OUTPUT)
    set(LUASF_LUA_STUB_OUTPUT "${LUASF_BINARY_ROOT}/LuaSF.d.lua")
endif()
set(LUASF_LUA_STUB_OUTPUT "${LUASF_LUA_STUB_OUTPUT}" CACHE FILEPATH "LuaSF Lua stub output path")

# When LuaSF is used through add_subdirectory(), the parent project owns the
# active configuration. CLion profiles, CMAKE_BUILD_TYPE, and --config Debug /
# Release will flow through normally. This default only applies when output/ is
# configured directly as the top-level project.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(LUASF_DEFAULT_CONFIG "Release" CACHE STRING "Default build configuration for the standalone LuaSF project")
    # Uncomment this line to make standalone LuaSF builds default to Debug.
    # set(LUASF_DEFAULT_CONFIG "Debug" CACHE STRING "Default build configuration for the standalone LuaSF project" FORCE)
    set_property(CACHE LUASF_DEFAULT_CONFIG PROPERTY STRINGS Release Debug RelWithDebInfo MinSizeRel)

    if(CMAKE_CONFIGURATION_TYPES)
        set(_luasf_configuration_types "${LUASF_DEFAULT_CONFIG};${CMAKE_CONFIGURATION_TYPES}")
        list(REMOVE_DUPLICATES _luasf_configuration_types)
        set(CMAKE_CONFIGURATION_TYPES "${_luasf_configuration_types}" CACHE STRING "LuaSF build configurations" FORCE)
    elseif(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "${LUASF_DEFAULT_CONFIG}" CACHE STRING "LuaSF build type" FORCE)
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Release Debug RelWithDebInfo MinSizeRel)
    endif()

    message(STATUS "LuaSF default build configuration: ${LUASF_DEFAULT_CONFIG}")
endif()

set(BUILD_SHARED_LIBS ${LUASF_BUILD_SHARED_SFML})
set(SFML_BUILD_EXAMPLES OFF)
set(SFML_BUILD_DOC OFF)
set(SFML_BUILD_TEST_SUITE OFF)

if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "Minimum macOS deployment target")
endif()

if(NOT TARGET sfml-system)
    add_subdirectory("${LUASF_SFML_ROOT}" "${LUASF_BINARY_ROOT}/third_party/SFML")
endif()

set(LUASF_LUA_TARGET "LuaSF_lua_shared")
if(NOT TARGET ${LUASF_LUA_TARGET})
    file(GLOB LUA_SOURCES CONFIGURE_DEPENDS
        "${LUASF_LUA_ROOT}/src/*.c"
    )
    list(REMOVE_ITEM LUA_SOURCES
        "${LUASF_LUA_ROOT}/src/lua.c"
        "${LUASF_LUA_ROOT}/src/luac.c"
        "${LUASF_LUA_ROOT}/src/onelua.c"
        "${LUASF_LUA_ROOT}/src/ltests.c"
    )

    add_library(${LUASF_LUA_TARGET} ${LUASF_LIBRARY_TYPE} ${LUA_SOURCES})
    target_include_directories(${LUASF_LUA_TARGET} PUBLIC
        "${LUASF_LUA_ROOT}/src"
    )
    if(WIN32)
        target_compile_definitions(${LUASF_LUA_TARGET}
            PRIVATE LUA_BUILD_AS_DLL LUA_CORE
            INTERFACE LUA_BUILD_AS_DLL
        )
    elseif(CMAKE_SYSTEM_NAME STREQUAL "iOS")
        target_compile_definitions(${LUASF_LUA_TARGET} PRIVATE LUA_USE_IOS)
    elseif(CMAKE_SYSTEM_NAME STREQUAL "OHOS")
        target_compile_definitions(${LUASF_LUA_TARGET} PRIVATE LUA_USE_POSIX)
        target_link_libraries(${LUASF_LUA_TARGET} PRIVATE m)
    elseif(APPLE)
        target_compile_definitions(${LUASF_LUA_TARGET} PRIVATE LUA_USE_MACOSX)
    elseif(UNIX)
        target_compile_definitions(${LUASF_LUA_TARGET} PRIVATE LUA_USE_LINUX)
        target_link_libraries(${LUASF_LUA_TARGET} PRIVATE m ${CMAKE_DL_LIBS})
    endif()
    set_target_properties(${LUASF_LUA_TARGET} PROPERTIES
        C_STANDARD 99
        C_STANDARD_REQUIRED ON
        OUTPUT_NAME "lua"
        RUNTIME_OUTPUT_DIRECTORY "${LUASF_BINARY_ROOT}/bin"
        BUILD_WITH_INSTALL_RPATH FALSE
        BUILD_RPATH "$<TARGET_FILE_DIR:LuaGlue::LuaGlue>;$<TARGET_FILE_DIR:${LUASF_LUA_TARGET}>;$<TARGET_FILE_DIR:sfml-system>"
        LIBRARY_OUTPUT_DIRECTORY "${LUASF_BINARY_ROOT}/bin"
        ARCHIVE_OUTPUT_DIRECTORY "${LUASF_BINARY_ROOT}/lib"
    )
endif()

if(NOT CMAKE_CROSSCOMPILING AND
   NOT CMAKE_SYSTEM_NAME STREQUAL "iOS" AND
   NOT CMAKE_SYSTEM_NAME STREQUAL "OHOS")
    add_executable(LuaSF_luac
        "${LUASF_LUA_ROOT}/src/luac.c"
        ${LUA_SOURCES}
    )
    target_include_directories(LuaSF_luac PRIVATE
        "${LUASF_LUA_ROOT}/src"
    )
    if(APPLE)
        target_compile_definitions(LuaSF_luac PRIVATE LUA_USE_MACOSX)
    elseif(UNIX)
        target_compile_definitions(LuaSF_luac PRIVATE LUA_USE_LINUX)
        target_link_libraries(LuaSF_luac PRIVATE m ${CMAKE_DL_LIBS})
    endif()
    set_target_properties(LuaSF_luac PROPERTIES
        C_STANDARD 99
        C_STANDARD_REQUIRED ON
        OUTPUT_NAME "luac"
        RUNTIME_OUTPUT_DIRECTORY "${LUASF_BINARY_ROOT}/tools"
    )
endif()

set(LUASF_BINDING_SOURCES
    "src/bind_Drawable.cpp"
    "src/bind_Handle.cpp"
    "src/System/bind_Angle.cpp"
    "src/System/bind_InputStream.cpp"
    "src/System/bind_FileInputStream.cpp"
    "src/System/bind_MemoryInputStream.cpp"
    "src/System/bind_Time.cpp"
    "src/System/bind_Clock.cpp"
    "src/System/bind_Sleep.cpp"
    "src/System/bind_TimeoutWithPredicate.cpp"
    "src/System/bind_Vector2.cpp"
    "src/System/bind_Vector3.cpp"
    "src/System/bind_Version.cpp"
    "src/Window/bind_Clipboard.cpp"
    "src/Window/bind_ContextSettings.cpp"
    "src/Window/bind_Context.cpp"
    "src/Window/bind_Cursor.cpp"
    "src/Window/bind_Joystick.cpp"
    "src/Window/bind_Keyboard.cpp"
    "src/Window/bind_Mouse.cpp"
    "src/Window/bind_Sensor.cpp"
    "src/bind_Event.cpp"
    "src/Window/bind_Touch.cpp"
    "src/Window/bind_VideoMode.cpp"
    "src/Window/bind_WindowEnums.cpp"
    "src/Window/bind_WindowBase.cpp"
    "src/Window/bind_Window.cpp"
    "src/Window/bind_WindowHandle.cpp"
    "src/Graphics/bind_BlendMode.cpp"
    "src/Graphics/bind_Color.cpp"
    "src/Graphics/bind_CoordinateType.cpp"
    "src/Graphics/bind_Image.cpp"
    "src/Graphics/bind_PrimitiveType.cpp"
    "src/Graphics/bind_Rect.cpp"
    "src/Graphics/bind_Glyph.cpp"
    "src/Graphics/bind_Shader.cpp"
    "src/Graphics/bind_StencilMode.cpp"
    "src/Graphics/bind_Texture.cpp"
    "src/Graphics/bind_Font.cpp"
    "src/Graphics/bind_Transform.cpp"
    "src/Graphics/bind_Glsl.cpp"
    "src/Graphics/bind_RenderStates.cpp"
    "src/Graphics/bind_Transformable.cpp"
    "src/Graphics/bind_Shape.cpp"
    "src/Graphics/bind_CircleShape.cpp"
    "src/Graphics/bind_ConvexShape.cpp"
    "src/Graphics/bind_RectangleShape.cpp"
    "src/Graphics/bind_Sprite.cpp"
    "src/bind_ClassSupport.cpp"
    "src/Graphics/bind_Vertex.cpp"
    "src/Graphics/bind_VertexArray.cpp"
    "src/Graphics/bind_Text.cpp"
    "src/Graphics/bind_VertexBuffer.cpp"
    "src/Graphics/bind_View.cpp"
    "src/Graphics/bind_RenderTarget.cpp"
    "src/Graphics/bind_RenderTexture.cpp"
    "src/Graphics/bind_RenderWindow.cpp"
    "src/Audio/bind_Listener.cpp"
    "src/Audio/bind_OutputSoundFile.cpp"
    "src/Audio/bind_PlaybackDevice.cpp"
    "src/Audio/bind_SoundChannel.cpp"
    "src/Audio/bind_InputSoundFile.cpp"
    "src/Audio/bind_SoundBuffer.cpp"
    "src/Audio/bind_SoundFileReader.cpp"
    "src/Audio/bind_SoundFileWriter.cpp"
    "src/Audio/bind_SoundRecorder.cpp"
    "src/Audio/bind_SoundBufferRecorder.cpp"
    "src/Audio/bind_SoundSource.cpp"
    "src/Audio/bind_Sound.cpp"
    "src/Audio/bind_SoundStream.cpp"
    "src/Audio/bind_Music.cpp"
    "src/Network/bind_IpAddress.cpp"
    "src/Network/bind_Dns.cpp"
    "src/Network/bind_Ftp.cpp"
    "src/Network/bind_Http.cpp"
    "src/Network/bind_Packet.cpp"
    "src/Network/bind_Sftp.cpp"
    "src/Network/bind_Socket.cpp"
    "src/Network/bind_SocketSelector.cpp"
    "src/Network/bind_TcpListener.cpp"
    "src/Network/bind_TcpSocket.cpp"
    "src/Network/bind_UdpSocket.cpp"
)

set(LUASF_RUNTIME_SOURCES
    "src/LuaCallbackCodec.cpp"
    "src/LuaStateLifecycle.cpp"
)

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin" AND CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    if(CMAKE_OSX_ARCHITECTURES STREQUAL "arm64" OR
       (NOT CMAKE_OSX_ARCHITECTURES AND CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64"))
        set_property(SOURCE ${LUASF_BINDING_SOURCES} "LuaSF.cpp" APPEND PROPERTY
            COMPILE_OPTIONS "$<$<CONFIG:Release>:-Os>"
        )
    endif()
endif()

if(NOT TARGET LuaGlue::LuaGlue)
    if(NOT EXISTS "${LUASF_GLUE_ROOT}/CMakeLists.txt")
        message(FATAL_ERROR "LuaGlue source project was not found: ${LUASF_GLUE_ROOT}")
    endif()
    set(LUAGLUE_LUA_TARGET "${LUASF_LUA_TARGET}")
    if(LUASF_LIBRARY_TYPE STREQUAL "SHARED")
        set(LUAGLUE_BUILD_SHARED ON)
    else()
        set(LUAGLUE_BUILD_SHARED OFF)
    endif()
    add_subdirectory("${LUASF_GLUE_ROOT}" "${LUASF_BINARY_ROOT}/LuaGlue")
endif()

set(LUASF_BINDING_HEADERS
    "include/LuaCallbackCodec.hpp"
    "include/LuaSFValueTraits.hpp"
    "include/LuaStateLifecycle.hpp"
    "include/bind_ClassSupport.hpp"
    "include/bind_Drawable.hpp"
    "include/bind_Event.hpp"
    "include/bind_Handle.hpp"
    "include/lua_stub.hpp"
    "include/luasf_glue.hpp"
    "include/utils.hpp"
    "include/utils.inl"
    "include/System/bind_Angle.hpp"
    "include/System/bind_InputStream.hpp"
    "include/System/bind_FileInputStream.hpp"
    "include/System/bind_MemoryInputStream.hpp"
    "include/System/bind_Time.hpp"
    "include/System/bind_Clock.hpp"
    "include/System/bind_Sleep.hpp"
    "include/System/bind_TimeoutWithPredicate.hpp"
    "include/System/bind_Vector2.hpp"
    "include/System/bind_Vector3.hpp"
    "include/System/bind_Version.hpp"
    "include/Window/bind_Clipboard.hpp"
    "include/Window/bind_ContextSettings.hpp"
    "include/Window/bind_Context.hpp"
    "include/Window/bind_Cursor.hpp"
    "include/Window/bind_Joystick.hpp"
    "include/Window/bind_Keyboard.hpp"
    "include/Window/bind_Mouse.hpp"
    "include/Window/bind_Sensor.hpp"
    "include/Window/bind_Touch.hpp"
    "include/Window/bind_VideoMode.hpp"
    "include/Window/bind_WindowEnums.hpp"
    "include/Window/bind_WindowBase.hpp"
    "include/Window/bind_Window.hpp"
    "include/Window/bind_WindowHandle.hpp"
    "include/Graphics/bind_BlendMode.hpp"
    "include/Graphics/bind_Color.hpp"
    "include/Graphics/bind_CoordinateType.hpp"
    "include/Graphics/bind_Image.hpp"
    "include/Graphics/bind_PrimitiveType.hpp"
    "include/Graphics/bind_Rect.hpp"
    "include/Graphics/bind_Glyph.hpp"
    "include/Graphics/bind_Shader.hpp"
    "include/Graphics/bind_StencilMode.hpp"
    "include/Graphics/bind_Texture.hpp"
    "include/Graphics/bind_Font.hpp"
    "include/Graphics/bind_Transform.hpp"
    "include/Graphics/bind_Glsl.hpp"
    "include/Graphics/bind_RenderStates.hpp"
    "include/Graphics/bind_Transformable.hpp"
    "include/Graphics/bind_Shape.hpp"
    "include/Graphics/bind_CircleShape.hpp"
    "include/Graphics/bind_ConvexShape.hpp"
    "include/Graphics/bind_RectangleShape.hpp"
    "include/Graphics/bind_Sprite.hpp"
    "include/Graphics/bind_Vertex.hpp"
    "include/Graphics/bind_VertexArray.hpp"
    "include/Graphics/bind_Text.hpp"
    "include/Graphics/bind_VertexBuffer.hpp"
    "include/Graphics/bind_View.hpp"
    "include/Graphics/bind_RenderTarget.hpp"
    "include/Graphics/bind_RenderTexture.hpp"
    "include/Graphics/bind_RenderWindow.hpp"
    "include/Audio/bind_Listener.hpp"
    "include/Audio/bind_OutputSoundFile.hpp"
    "include/Audio/bind_PlaybackDevice.hpp"
    "include/Audio/bind_SoundChannel.hpp"
    "include/Audio/bind_InputSoundFile.hpp"
    "include/Audio/bind_SoundBuffer.hpp"
    "include/Audio/bind_SoundFileReader.hpp"
    "include/Audio/bind_SoundFileWriter.hpp"
    "include/Audio/bind_SoundRecorder.hpp"
    "include/Audio/bind_SoundBufferRecorder.hpp"
    "include/Audio/bind_SoundSource.hpp"
    "include/Audio/bind_Sound.hpp"
    "include/Audio/bind_SoundStream.hpp"
    "include/Audio/bind_Music.hpp"
    "include/Network/bind_IpAddress.hpp"
    "include/Network/bind_Dns.hpp"
    "include/Network/bind_Ftp.hpp"
    "include/Network/bind_Http.hpp"
    "include/Network/bind_Packet.hpp"
    "include/Network/bind_Sftp.hpp"
    "include/Network/bind_Socket.hpp"
    "include/Network/bind_SocketSelector.hpp"
    "include/Network/bind_TcpListener.hpp"
    "include/Network/bind_TcpSocket.hpp"
    "include/Network/bind_UdpSocket.hpp"
    "include/LuaSF.hpp"
)

add_library(LuaSF_bindings OBJECT
    ${LUASF_BINDING_SOURCES}
    ${LUASF_RUNTIME_SOURCES}
    "LuaSF.cpp"
    ${LUASF_BINDING_HEADERS}
)

add_library(LuaSF ${LUASF_LIBRARY_TYPE}
    $<TARGET_OBJECTS:LuaSF_bindings>
)

function(luasf_configure_binding_target target_name output_dir archive_dir)
    set(_luasf_binding_objects "LuaSF_bindings")

    target_compile_features("${_luasf_binding_objects}" PRIVATE cxx_std_20)

    target_include_directories("${_luasf_binding_objects}"
        PRIVATE
            "${LUASF_ROOT}/include"
            "${LUASF_SFML_ROOT}/include"
            "${LUASF_LUA_ROOT}/src"
    )

    target_link_libraries("${_luasf_binding_objects}" PRIVATE LuaGlue::LuaGlue)

    target_compile_definitions("${_luasf_binding_objects}" PRIVATE
        LUASF_BUILD_DLL=1
    )

    # Visibility and LTO belong to the objects: the shared module is only a
    # link step over them, and the stub writer links the same objects.
    set_target_properties("${_luasf_binding_objects}" PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
        INTERPROCEDURAL_OPTIMIZATION_RELEASE ${LUASF_RELEASE_LTO}
    )

    if(MSVC)
        target_compile_options("${_luasf_binding_objects}" PRIVATE /permissive- /EHsc /bigobj /utf-8)
    else()
        target_compile_options("${_luasf_binding_objects}" PRIVATE -Wall -Wextra)
    endif()

    target_compile_features("${target_name}" PUBLIC cxx_std_20)

    target_include_directories("${target_name}"
        PUBLIC
            "$<BUILD_INTERFACE:${LUASF_ROOT}/include>"
            "$<BUILD_INTERFACE:${LUASF_SFML_ROOT}/include>"
    )

    target_link_libraries("${target_name}" PUBLIC LuaGlue::LuaGlue PRIVATE
        ${LUASF_LUA_TARGET}
        sfml-system
        sfml-window
        sfml-graphics
        sfml-audio
        sfml-network
    )

    set_target_properties("${target_name}" PROPERTIES
        PREFIX ""
        OUTPUT_NAME "LuaSF"
        INTERPROCEDURAL_OPTIMIZATION_RELEASE ${LUASF_RELEASE_LTO}
        LIBRARY_OUTPUT_DIRECTORY "${output_dir}"
        RUNTIME_OUTPUT_DIRECTORY "${output_dir}"
        ARCHIVE_OUTPUT_DIRECTORY "${archive_dir}"
    )

    if(WIN32)
        set_target_properties("${target_name}" PROPERTIES SUFFIX ".dll")
    endif()
endfunction()

luasf_configure_binding_target(
    LuaSF
    "${LUASF_EMBEDDED_OUTPUT_DIR}"
    "${LUASF_EMBEDDED_ARCHIVE_DIR}"
)

if(NOT TARGET LuaSF::LuaSF)
    add_library(LuaSF::LuaSF ALIAS LuaSF)
endif()

function(luasf_copy_target_runtime_files target_name include_lua_runtime)
    if(NOT TARGET "${target_name}")
        message(FATAL_ERROR "luasf_copy_target_runtime_files target does not exist: ${target_name}")
    endif()

    get_target_property(_luasf_library_type LuaSF TYPE)
    if(_luasf_library_type STREQUAL "STATIC_LIBRARY")
        return()
    endif()

    set(_luasf_runtime_files)
    if(include_lua_runtime)
        list(APPEND _luasf_runtime_files "$<TARGET_FILE:LuaSF_lua_shared>")
    endif()
    foreach(_luasf_runtime_target IN ITEMS
        LuaGlue::LuaGlue
        sfml-system
        sfml-window
        sfml-graphics
        sfml-audio
        sfml-network
    )
        if(TARGET "${_luasf_runtime_target}")
            list(APPEND _luasf_runtime_files "$<TARGET_FILE:${_luasf_runtime_target}>")
        endif()
    endforeach()

    add_custom_command(TARGET "${target_name}" POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            ${_luasf_runtime_files}
            "$<TARGET_FILE_DIR:${target_name}>"
        VERBATIM
    )
endfunction()

function(luasf_copy_runtime_files target_name)
    if(NOT TARGET "${target_name}")
        message(FATAL_ERROR "luasf_copy_runtime_files target does not exist: ${target_name}")
    endif()

    get_target_property(_luasf_library_type LuaSF TYPE)
    if(_luasf_library_type STREQUAL "STATIC_LIBRARY")
        return()
    endif()

    set(_luasf_runtime_files
        "$<TARGET_FILE:LuaSF>"
        "$<TARGET_FILE:LuaSF_lua_shared>"
    )
    foreach(_luasf_runtime_target IN ITEMS
        LuaGlue::LuaGlue
        sfml-system
        sfml-window
        sfml-graphics
        sfml-audio
        sfml-network
    )
        if(TARGET "${_luasf_runtime_target}")
            list(APPEND _luasf_runtime_files "$<TARGET_FILE:${_luasf_runtime_target}>")
        endif()
    endforeach()

    add_custom_command(TARGET "${target_name}" POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            ${_luasf_runtime_files}
            "$<TARGET_FILE_DIR:${target_name}>"
        VERBATIM
    )
endfunction()

function(luasf_copy_runtime_dlls target_name)
    luasf_copy_runtime_files("${target_name}")
endfunction()

luasf_copy_target_runtime_files(LuaSF TRUE)

function(luasf_add_lua_stub target_name)
    if(CMAKE_CROSSCOMPILING)
        return()
    endif()

    if(NOT LUASF_GENERATE_LUA_STUB)
        return()
    endif()

    add_executable(${target_name}_lua_stub_dump EXCLUDE_FROM_ALL
        "${LUASF_ROOT}/tools/lua_stub_dump.cpp"
    )
    target_compile_features(${target_name}_lua_stub_dump PRIVATE cxx_std_20)
    target_compile_definitions(${target_name}_lua_stub_dump PRIVATE
        LUASF_BUILD_DLL=1
    )
    target_include_directories(${target_name}_lua_stub_dump PRIVATE
        "${LUASF_ROOT}/include"
        "${LUASF_LUA_ROOT}/src"
    )
    target_link_libraries(${target_name}_lua_stub_dump PRIVATE
        "LuaSF_bindings"
        LuaGlue::LuaGlue
        ${LUASF_LUA_TARGET}
        sfml-system
        sfml-window
        sfml-graphics
        sfml-audio
        sfml-network
    )
    set_target_properties(${target_name}_lua_stub_dump PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${LUASF_BINARY_ROOT}/bin"
        BUILD_WITH_INSTALL_RPATH FALSE
        BUILD_RPATH "$<TARGET_FILE_DIR:LuaGlue::LuaGlue>;$<TARGET_FILE_DIR:${LUASF_LUA_TARGET}>;$<TARGET_FILE_DIR:sfml-system>"
    )

    set(_luasf_stub_args "${LUASF_LUA_STUB_OUTPUT}")

    if(WIN32)
        add_custom_command(TARGET ${target_name}_lua_stub_dump POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "$<TARGET_FILE:${LUASF_LUA_TARGET}>"
                "$<TARGET_FILE:LuaGlue::LuaGlue>"
                "$<TARGET_FILE:sfml-system>"
                "$<TARGET_FILE:sfml-window>"
                "$<TARGET_FILE:sfml-graphics>"
                "$<TARGET_FILE:sfml-audio>"
                "$<TARGET_FILE:sfml-network>"
                "$<TARGET_FILE_DIR:${target_name}_lua_stub_dump>"
            VERBATIM
        )
        set(_luasf_stub_command
            "$<TARGET_FILE:${target_name}_lua_stub_dump>"
            ${_luasf_stub_args}
        )
    else()
        if(APPLE)
            set(_luasf_stub_command
                "${CMAKE_COMMAND}" -E env
                "DYLD_LIBRARY_PATH=$<TARGET_FILE_DIR:LuaGlue::LuaGlue>:$<TARGET_FILE_DIR:${LUASF_LUA_TARGET}>:$<TARGET_FILE_DIR:sfml-system>:$ENV{DYLD_LIBRARY_PATH}"
                "$<TARGET_FILE:${target_name}_lua_stub_dump>"
                ${_luasf_stub_args}
            )
        else()
            set(_luasf_stub_command
                "${CMAKE_COMMAND}" -E env
                "LD_LIBRARY_PATH=$<TARGET_FILE_DIR:LuaGlue::LuaGlue>:$<TARGET_FILE_DIR:${LUASF_LUA_TARGET}>:$<TARGET_FILE_DIR:sfml-system>:$ENV{LD_LIBRARY_PATH}"
                "$<TARGET_FILE:${target_name}_lua_stub_dump>"
                ${_luasf_stub_args}
            )
        endif()
    endif()

    add_dependencies(${target_name}_lua_stub_dump
        "LuaSF_bindings"
        LuaGlue::LuaGlue
        ${LUASF_LUA_TARGET}
        sfml-system
        sfml-window
        sfml-graphics
        sfml-audio
        sfml-network
    )

    add_dependencies(${target_name} ${target_name}_lua_stub_dump)

    add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E make_directory "$<SHELL_PATH:${LUASF_BINARY_ROOT}>"
        COMMAND ${_luasf_stub_command}
        VERBATIM
        COMMENT "Generating LuaSF Lua stub"
    )

    add_custom_target(${target_name}_lua_stub
        COMMAND "${CMAKE_COMMAND}" -E make_directory "$<SHELL_PATH:${LUASF_BINARY_ROOT}>"
        COMMAND ${_luasf_stub_command}
        DEPENDS ${target_name} ${target_name}_lua_stub_dump
        VERBATIM
        COMMENT "Generating LuaSF Lua stub"
    )
endfunction()

luasf_add_lua_stub(LuaSF)

add_custom_target(LuaSF_build_outputs
    DEPENDS LuaSF
)
if(TARGET LuaSF_luac)
    add_dependencies(LuaSF LuaSF_luac)
    add_dependencies(LuaSF_build_outputs LuaSF_luac)
endif()
if(TARGET LuaSF_lua_stub)
    add_dependencies(LuaSF_build_outputs LuaSF_lua_stub)
endif()
