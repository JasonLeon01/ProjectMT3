set(INCROOT ${PROJECT_SOURCE_DIR}/include/SFML/Graphics)
set(SRCROOT ${PROJECT_SOURCE_DIR}/src/SFML/Graphics)

# all source files
set(SRC
    ${SRCROOT}/BlendMode.cpp
    ${INCROOT}/BlendMode.hpp
    ${INCROOT}/Color.hpp
    ${INCROOT}/Color.inl
    ${INCROOT}/CoordinateType.hpp
    ${INCROOT}/Export.hpp
    ${SRCROOT}/Font.cpp
    ${SRCROOT}/GraphicsBackend.cpp
    ${INCROOT}/RHI.hpp
    ${INCROOT}/RHI/Device.hpp
    ${SRCROOT}/RHI/Resource.cpp
    ${SRCROOT}/RHI/Resource.hpp
    ${SRCROOT}/RHI/Access.hpp
    ${SRCROOT}/RHI/Access.cpp
    ${SRCROOT}/RHI/NativeResources.hpp
    ${SRCROOT}/GraphicsDevice.hpp
    ${SRCROOT}/RenderTargetBackend.hpp
    ${SRCROOT}/ShaderUniformType.hpp
    ${INCROOT}/GraphicsBackend.hpp
    ${INCROOT}/Font.hpp
    ${SRCROOT}/Glsl.cpp
    ${INCROOT}/Glsl.hpp
    ${INCROOT}/Glsl.inl
    ${INCROOT}/Glyph.hpp
    ${SRCROOT}/Image.cpp
    ${INCROOT}/Image.hpp
    ${INCROOT}/PrimitiveType.hpp
    ${INCROOT}/Rect.hpp
    ${INCROOT}/Rect.inl
    ${SRCROOT}/RenderStates.cpp
    ${INCROOT}/RenderStates.hpp
    ${SRCROOT}/RenderTexture.cpp
    ${INCROOT}/RenderTexture.hpp
    ${SRCROOT}/RenderTarget.cpp
    ${INCROOT}/RenderTarget.hpp
    ${SRCROOT}/RenderWindow.cpp
    ${INCROOT}/RenderWindow.hpp
    ${SRCROOT}/Shader.cpp
    ${INCROOT}/Shader.hpp
    ${SRCROOT}/StencilMode.cpp
    ${INCROOT}/StencilMode.hpp
    ${SRCROOT}/Texture.cpp
    ${INCROOT}/Texture.hpp
    ${SRCROOT}/Transform.cpp
    ${INCROOT}/Transform.hpp
    ${INCROOT}/Transform.inl
    ${SRCROOT}/Transformable.cpp
    ${INCROOT}/Transformable.hpp
    ${SRCROOT}/View.cpp
    ${INCROOT}/View.hpp
    ${INCROOT}/Vertex.hpp
)
source_group("" FILES ${SRC})

# drawables sources
set(DRAWABLES_SRC
    ${INCROOT}/Drawable.hpp
    ${SRCROOT}/Shape.cpp
    ${INCROOT}/Shape.hpp
    ${SRCROOT}/CircleShape.cpp
    ${INCROOT}/CircleShape.hpp
    ${SRCROOT}/RectangleShape.cpp
    ${INCROOT}/RectangleShape.hpp
    ${SRCROOT}/ConvexShape.cpp
    ${INCROOT}/ConvexShape.hpp
    ${SRCROOT}/Sprite.cpp
    ${INCROOT}/Sprite.hpp
    ${SRCROOT}/Text.cpp
    ${INCROOT}/Text.hpp
    ${SRCROOT}/VertexArray.cpp
    ${INCROOT}/VertexArray.hpp
    ${SRCROOT}/VertexBuffer.cpp
    ${INCROOT}/VertexBuffer.hpp
)
source_group("drawables" FILES ${DRAWABLES_SRC})

# render-texture sources
set(RENDER_TEXTURE_SRC
    ${SRCROOT}/RenderTextureImpl.hpp
    ${SRCROOT}/RenderTextureImplFBO.cpp
    ${SRCROOT}/RenderTextureImplFBO.hpp
    ${SRCROOT}/RenderTextureImplDefault.cpp
    ${SRCROOT}/RenderTextureImplDefault.hpp
)
source_group("render texture" FILES ${RENDER_TEXTURE_SRC})


# define the sfml-graphics target
sfml_add_library(Graphics
                 SOURCES ${SRC} ${DRAWABLES_SRC}
                 DEPENDENCIES "Dependencies.cmake.in")

# setup dependencies
target_link_libraries(sfml-graphics PUBLIC SFML::Window)

if(SFML_BUILD_DIRECTX_BACKEND)
    target_compile_definitions(sfml-graphics PUBLIC SFML_RENDER_DIRECTX)
    target_link_libraries(sfml-graphics PRIVATE d3d11 dxgi dxguid)
    target_sources(sfml-graphics PRIVATE
        ${SRCROOT}/DirectX/DirectXReadback.cpp
        ${SRCROOT}/DirectX/DirectXTimer.cpp
        ${SRCROOT}/DirectX/DirectXRHI.cpp
        ${SRCROOT}/DirectX/DirectXRHI.hpp
        ${SRCROOT}/DirectX/DirectXDevice.cpp
        ${SRCROOT}/DirectX/DirectXDevice.hpp
        ${SRCROOT}/DirectX/DirectXProgram.cpp
        ${SRCROOT}/DirectX/DirectXProgram.hpp
        ${SRCROOT}/DirectX/DirectXRenderTarget.cpp
        ${SRCROOT}/DirectX/DirectXRenderTarget.hpp
        ${SRCROOT}/DirectX/DirectXWindowSurface.cpp
        ${SRCROOT}/DirectX/DirectXWindowSurface.hpp
        ${SRCROOT}/DirectX/ComPtr.hpp
        ${SRCROOT}/DirectX/BuiltInShaders.hpp
    )
    if(SFML_BUILD_TEST_SUITE)
        target_compile_definitions(sfml-graphics PRIVATE SFML_DIRECTX_TESTS)
    endif()
endif()

if(SFML_BUILD_METAL_BACKEND)
    enable_language(OBJCXX)
    set(METAL_SRC
        ${SRCROOT}/Metal/BuiltInShaders.hpp
        ${SRCROOT}/Metal/MetalDevice.hpp
        ${SRCROOT}/Metal/MetalRHI.mm
        ${SRCROOT}/Metal/MetalRHI.hpp
        ${SRCROOT}/Metal/MetalReadback.hpp
        ${SRCROOT}/Metal/MetalReadback.mm
        ${SRCROOT}/Metal/MetalTimer.hpp
        ${SRCROOT}/Metal/MetalTimer.mm
        ${SRCROOT}/Metal/MetalDevice.mm
        ${SRCROOT}/Metal/MetalObjects.hpp
        ${SRCROOT}/Metal/MetalProgram.hpp
        ${SRCROOT}/Metal/MetalProgram.mm
        ${SRCROOT}/Metal/MetalRenderTarget.hpp
        ${SRCROOT}/Metal/MetalRenderTarget.mm
        ${SRCROOT}/Metal/MetalWindowSurface.hpp
        ${SRCROOT}/Metal/MetalWindowSurface.mm
    )
    target_sources(sfml-graphics PRIVATE ${METAL_SRC})
    source_group("metal" FILES ${METAL_SRC})
    set_source_files_properties(${METAL_SRC} PROPERTIES COMPILE_OPTIONS "-fobjc-arc" SKIP_UNITY_BUILD_INCLUSION ON)
    target_compile_definitions(sfml-graphics PUBLIC SFML_RENDER_METAL)
    target_link_libraries(sfml-graphics PRIVATE "-framework Foundation" "-framework Metal" "-framework QuartzCore")
endif()

if(SFML_BUILD_OPENGL_BACKEND)
    target_compile_definitions(sfml-graphics PRIVATE SFML_RENDER_GL)
    target_include_directories(sfml-graphics SYSTEM PRIVATE "${PROJECT_SOURCE_DIR}/extlibs/headers/glad/include")
    target_sources(sfml-graphics PRIVATE
        ${SRCROOT}/GLCheck.cpp
        ${SRCROOT}/GLCheck.hpp
        ${SRCROOT}/GLExtensions.cpp
        ${SRCROOT}/GLExtensions.hpp
        ${SRCROOT}/TextureSaver.cpp
        ${SRCROOT}/TextureSaver.hpp
        ${SRCROOT}/OpenGL/OpenGLRenderTarget.cpp
        ${SRCROOT}/OpenGL/OpenGLRenderTarget.hpp
        ${SRCROOT}/OpenGL/OpenGLResource.hpp
        ${SRCROOT}/OpenGL/OpenGLVertexArray.cpp
        ${SRCROOT}/OpenGL/OpenGLReadback.cpp
        ${SRCROOT}/OpenGL/OpenGLTimer.cpp
        ${SRCROOT}/OpenGL/OpenGLRHI.cpp
        ${SRCROOT}/OpenGL/OpenGLTexture.cpp
        ${SRCROOT}/OpenGL/OpenGLTexture.hpp
        ${SRCROOT}/OpenGL/OpenGLBuffer.cpp
        ${SRCROOT}/OpenGL/OpenGLBuffer.hpp
        ${SRCROOT}/OpenGL/OpenGLProgram.cpp
        ${SRCROOT}/OpenGL/OpenGLProgram.hpp
        ${RENDER_TEXTURE_SRC}
    )
endif()

if(SFML_BUILD_VULKAN_BACKEND)
    target_compile_definitions(sfml-graphics PRIVATE VK_NO_PROTOTYPES)
    include(FetchContent)
    FetchContent_Declare(SpirvReflect
        URL https://codeload.github.com/KhronosGroup/SPIRV-Reflect/zip/refs/tags/vulkan-sdk-1.4.321.0
        URL_HASH SHA256=cc5f56f857c81ba1d14df2170667aba4ae652f970153f9f1b4c0ec566cea0ad8
        SOURCE_SUBDIR sfml-unused
    )
    FetchContent_MakeAvailable(SpirvReflect)
    add_library(sfml-spirv-reflect OBJECT "${spirvreflect_SOURCE_DIR}/spirv_reflect.c")
    set_target_properties(sfml-spirv-reflect PROPERTIES POSITION_INDEPENDENT_CODE ON UNITY_BUILD OFF FOLDER Dependencies)
    sfml_set_stdlib(sfml-spirv-reflect)
    target_compile_options(sfml-spirv-reflect PRIVATE -w)
    target_sources(sfml-graphics PRIVATE
        $<TARGET_OBJECTS:sfml-spirv-reflect>
        ${SRCROOT}/Vulkan/VulkanRHI.cpp
        ${SRCROOT}/Vulkan/VulkanRHI.hpp
        ${SRCROOT}/Vulkan/VulkanDevice.cpp
        ${SRCROOT}/Vulkan/VulkanDevice.hpp
        ${SRCROOT}/Vulkan/VulkanFunctions.inc
        ${SRCROOT}/Vulkan/VulkanProgram.cpp
        ${SRCROOT}/Vulkan/VulkanProgram.hpp
        ${SRCROOT}/Vulkan/VulkanRenderTarget.cpp
        ${SRCROOT}/Vulkan/VulkanRenderTarget.hpp
        ${SRCROOT}/Vulkan/VulkanWindowSurface.cpp
        ${SRCROOT}/Vulkan/VulkanWindowSurface.hpp
    )
    target_include_directories(sfml-graphics SYSTEM PRIVATE "${PROJECT_SOURCE_DIR}/extlibs/headers/vulkan" "${spirvreflect_SOURCE_DIR}")
    install(FILES "${spirvreflect_SOURCE_DIR}/LICENSE" DESTINATION ${CMAKE_INSTALL_DOCDIR} RENAME SPIRV-Reflect-LICENSE)
endif()

# stb_image sources
target_include_directories(sfml-graphics SYSTEM PRIVATE "${PROJECT_SOURCE_DIR}/extlibs/headers/stb_image")

# qoi sources
target_include_directories(sfml-graphics SYSTEM PRIVATE "${PROJECT_SOURCE_DIR}/extlibs/headers/qoi")

# find external libraries
if(SFML_OS_ANDROID OR SFML_OS_HARMONY)
    target_link_libraries(sfml-graphics PRIVATE z)
elseif(SFML_OS_IOS)
    target_link_libraries(sfml-graphics PRIVATE z bz2)
endif()

if(SFML_USE_SYSTEM_DEPS)
    find_package(Freetype REQUIRED)
    find_package(HarfBuzz REQUIRED)
else()
    # use a block to scope option variables we have to set
    block()
        include(FetchContent)

        # The FreeType <-> HarfBuzz relationship is a complex one.
        # HarfBuzz relies on FreeType to manage glyph data.
        # FreeType relies on HarfBuzz for improved auto-hinting.
        # Using FetchContent alone will not work since both libraries want the
        # other to already exist as targets at configuration time.
        # We break this dependency cycle by patching the FreeType configuration
        # to assume that HarfBuzz exists but don't do any target linking itself.
        # We import HarfBuzz normally, telling it to link to FreeType.
        # After the HarfBuzz target has been created, we manually link the
        # FreeType target to HarfBuzz so the include directories and linker paths
        # to HarfBuzz are set for the FreeType library as well.

        set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
        set(CMAKE_DEBUG_POSTFIX d)
        set(BUILD_SHARED_LIBS OFF)
        set(SKIP_INSTALL_HEADERS ON)
        set(FT_DISABLE_ZLIB ON)
        set(FT_DISABLE_BZIP2 ON)
        set(FT_DISABLE_PNG ON)
        set(FT_DISABLE_HARFBUZZ ON)
        set(FT_DISABLE_BROTLI ON)
        set(FT_ENABLE_ERROR_STRINGS ON)

        FetchContent_Declare(Freetype
            GIT_REPOSITORY https://github.com/freetype/freetype.git
            GIT_TAG VER-2-14-3
            GIT_SHALLOW ON
            OVERRIDE_FIND_PACKAGE
            # patch out parts we don't want of the FreeType CMake configuration
            # - installing pkgconfig files
            # - assume HarfBuzz exists without linking targets yet
            PATCH_COMMAND ${CMAKE_COMMAND} -DFREETYPE_DIR=${FETCHCONTENT_BASE_DIR}/freetype-src -P ${PROJECT_SOURCE_DIR}/tools/freetype/PatchFreetype.cmake)

        set(HB_BUILD_SUBSET OFF)
        set(HB_BUILD_RASTER OFF)
        set(HB_BUILD_VECTOR OFF)
        set(HB_BUILD_GPU OFF)
        set(HB_HAVE_FREETYPE ON)
        set(HB_HAVE_CORETEXT OFF)

        FetchContent_Declare(HarfBuzz
            GIT_REPOSITORY https://github.com/harfbuzz/harfbuzz.git
            GIT_TAG 14.1.0
            GIT_SHALLOW ON
            OVERRIDE_FIND_PACKAGE
            # patch out parts we don't want of the HarfBuzz CMake configuration
            # - unnecessary warnings
            # - fix target_link_libraries being called without specifying PUBLIC
            PATCH_COMMAND ${CMAKE_COMMAND} -DHARFBUZZ_DIR=${FETCHCONTENT_BASE_DIR}/harfbuzz-src -P ${PROJECT_SOURCE_DIR}/tools/harfbuzz/PatchHarfBuzz.cmake)

        FetchContent_MakeAvailable(Freetype HarfBuzz)
    endblock()

    # add HarfBuzz preprocessor symbols
    target_compile_definitions(harfbuzz PRIVATE
        HB_DISABLE_DEPRECATED
        HB_NO_ATEXIT
        HB_NO_BORING_EXPANSION
        HB_NO_BUFFER_MESSAGE
        HB_NO_BUFFER_SERIALIZE
        HB_NO_BUFFER_VERIFY
        HB_NO_BITMAP
        HB_NO_DRAW
        HB_NO_ERRNO
        HB_NO_GETENV
        HB_NO_MT
        HB_NO_PAINT
        HB_NO_SETLOCALE
        HB_OPTIMIZE_SIZE
    )

    # Now that both targets exist, add the HarfBuzz include directories to the freetype target
    # We can't use target_link_libraries here because creating a circular dependency between FreeType and HarfBuzz
    # will make it impossible to resolve either of them with find_dependency in the installed Graphics dependency configuration
    # because we break the link here ourselves, we will have to re-establish it in Dependencies.cmake so CMake knows
    # it has to adjust the linker command line by duplicating the libraries that have circular dependencies
    target_include_directories(freetype SYSTEM PRIVATE $<TARGET_PROPERTY:harfbuzz,INTERFACE_INCLUDE_DIRECTORIES>)

    set_target_properties(freetype harfbuzz PROPERTIES FOLDER "Dependencies" SYSTEM ON)

    # if building SFML as a shared library or on android (where exes are shared libs) and linking our dependencies in
    # as static libraries we need to build them with -fPIC
    if(BUILD_SHARED_LIBS OR SFML_OS_ANDROID)
        set_target_properties(freetype harfbuzz PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()

    # build using unity build and exclude files that cause issues
    set_target_properties(freetype PROPERTIES UNITY_BUILD ON UNITY_BUILD_BATCH_SIZE 256)
    set_source_files_properties(
        "${FETCHCONTENT_BASE_DIR}/freetype-src/src/pfr/pfr.c"
        "${FETCHCONTENT_BASE_DIR}/freetype-src/src/smooth/smooth.c"
        "${FETCHCONTENT_BASE_DIR}/freetype-src/builds/windows/ftsystem.c"
        "${FETCHCONTENT_BASE_DIR}/freetype-src/builds/windows/ftdebug.c"
        TARGET_DIRECTORY freetype
        PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON
    )

    # disable building dependencies as part of our unity build, they have their own internal unity build system
    set_target_properties(harfbuzz PROPERTIES UNITY_BUILD OFF)

    sfml_set_stdlib(freetype)
    sfml_set_stdlib(harfbuzz)

    # HarfBuzz builds using a unity build system, all its files are combined into harfbuzz.cc
    # when building using mingw this can lead to "too many sections" and "file too big" errors
    # thus we will have to activate the -bigobj assembler flag when building using mingw
    if(WIN32 AND SFML_COMPILER_GCC)
        target_compile_options(harfbuzz PRIVATE -Wa,-mbig-obj)
    endif()

    # Disable all warnings
    target_compile_options(freetype PRIVATE -w)
    target_compile_options(harfbuzz PRIVATE -w)

    add_library(Freetype::Freetype ALIAS freetype)
    add_library(HarfBuzz::HarfBuzz ALIAS harfbuzz)
endif()

target_link_libraries(sfml-graphics PRIVATE Freetype::Freetype HarfBuzz::HarfBuzz)

# we have to build SheenBidi ourselves in both cases since there is no system package for it
# use a block to scope option variables we have to set
block()
    include(FetchContent)

    set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
    set(BUILD_SHARED_LIBS OFF)

    FetchContent_Declare(SheenBidi
        GIT_REPOSITORY https://github.com/Tehreer/SheenBidi.git
        GIT_TAG v3.0.0
        GIT_SHALLOW ON
        # patch out parts we don't want of the SheenBidi CMake configuration
        # - disable installation
        # - turn the library into an object library so we can merge it into sfml-graphics
        PATCH_COMMAND ${CMAKE_COMMAND} -DSHEENBIDI_DIR=${FETCHCONTENT_BASE_DIR}/sheenbidi-src -P ${PROJECT_SOURCE_DIR}/tools/sheenbidi/PatchSheenBidi.cmake)
    FetchContent_MakeAvailable(SheenBidi)
endblock()

set_target_properties(SheenBidi PROPERTIES FOLDER "Dependencies")

# disable building SheenBidi as part of our unity build, it has its own internal unity build system
set_target_properties(SheenBidi PROPERTIES UNITY_BUILD OFF)

sfml_set_stdlib(SheenBidi)

# add SheenBidi object library and headers to our own target
target_sources(sfml-graphics PRIVATE $<TARGET_OBJECTS:SheenBidi>)
target_include_directories(sfml-graphics SYSTEM PRIVATE "${SheenBidi_SOURCE_DIR}/Headers")

# Disable all warnings
target_compile_options(SheenBidi PRIVATE -w)

# if building SFML as a shared library or on android (where exes are shared libs) and linking our dependencies in
# as static libraries we need to build them with -fPIC
if(BUILD_SHARED_LIBS OR SFML_OS_ANDROID)
    set_target_properties(SheenBidi PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# add preprocessor symbols
target_compile_definitions(sfml-graphics PRIVATE "STBI_FAILURE_USERMSG")

if(SFML_FATAL_OPENGL_ERRORS)
    target_compile_definitions(sfml-graphics PRIVATE "SFML_FATAL_OPENGL_ERRORS")
endif()

# Image.cpp must be compiled with the -fno-strict-aliasing
# when gcc is used; otherwise saving PNGs may crash in stb_image_write
if(SFML_COMPILER_GCC)
    set_source_files_properties(${SRCROOT}/Image.cpp PROPERTIES COMPILE_FLAGS -fno-strict-aliasing)
endif()
