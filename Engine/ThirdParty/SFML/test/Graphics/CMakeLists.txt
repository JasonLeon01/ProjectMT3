
set(GRAPHICS_SRC
    BlendMode.test.cpp
    CircleShape.test.cpp
    Color.test.cpp
    ConvexShape.test.cpp
    CoordinateType.test.cpp
    Drawable.test.cpp
    Font.test.cpp
    Glsl.test.cpp
    Glyph.test.cpp
    Image.test.cpp
    Rect.test.cpp
    RectangleShape.test.cpp
    Render.test.cpp
    RenderStates.test.cpp
    RenderTarget.test.cpp
    RenderTexture.test.cpp
    RenderWindow.test.cpp
    Shader.test.cpp
    Shape.test.cpp
    Sprite.test.cpp
    StencilMode.test.cpp
    Text.test.cpp
    Texture.test.cpp
    Transform.test.cpp
    Transformable.test.cpp
    Vertex.test.cpp
    VertexArray.test.cpp
    VertexBuffer.test.cpp
    View.test.cpp
)
if(SFML_OS_WINDOWS)
    list(APPEND GRAPHICS_SRC
        WindowHandleWin32.test.cpp
    )
endif()
if(SFML_BUILD_DIRECTX_BACKEND)
    list(APPEND GRAPHICS_SRC DirectX.test.cpp)
endif()
if(SFML_BUILD_METAL_BACKEND)
    list(APPEND GRAPHICS_SRC Metal.test.cpp)
endif()
sfml_add_test(test-sfml-graphics "${GRAPHICS_SRC}" SFML::Graphics)
target_compile_definitions(test-sfml-graphics PRIVATE SFML_GRAPHICS_TEST_MAIN)
if(SFML_BUILD_METAL_BACKEND AND SFML_OS_MACOS)
    # The metallib fixture is only built when the optional Metal Toolchain is installed
    execute_process(COMMAND xcrun -sdk macosx metal --version
                    RESULT_VARIABLE SFML_METAL_COMPILER_RESULT OUTPUT_QUIET ERROR_QUIET)
    if(SFML_METAL_COMPILER_RESULT EQUAL 0)
        set(METAL_LIBRARY_FIXTURE ${CMAKE_CURRENT_BINARY_DIR}/shader-metal.metallib)
        add_custom_command(OUTPUT ${METAL_LIBRARY_FIXTURE}
                           COMMAND xcrun -sdk macosx metal -mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET}
                                   -o ${METAL_LIBRARY_FIXTURE} ${CMAKE_CURRENT_SOURCE_DIR}/shader-metal.metal
                           DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shader-metal.metal
                           VERBATIM)
        target_sources(test-sfml-graphics PRIVATE ${METAL_LIBRARY_FIXTURE})
        target_compile_definitions(test-sfml-graphics PRIVATE SFML_METAL_LIBRARY_FIXTURE="${METAL_LIBRARY_FIXTURE}")
    endif()
    if(SFML_RUN_DISPLAY_TESTS)
        add_test(NAME sfml-graphics-metal COMMAND test-sfml-graphics --sfml-metal
                 WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
        set_tests_properties(sfml-graphics-metal PROPERTIES RUN_SERIAL TRUE)
    endif()
endif()
if(SFML_RUN_DISPLAY_TESTS AND SFML_BUILD_OPENGL_BACKEND AND SFML_BUILD_METAL_BACKEND AND SFML_OS_MACOS)
    add_test(NAME sfml-graphics-opengl COMMAND test-sfml-graphics --sfml-opengl
             WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    set_tests_properties(sfml-graphics-opengl PROPERTIES RUN_SERIAL TRUE)
endif()
if(SFML_BUILD_DIRECTX_BACKEND)
    target_compile_definitions(test-sfml-graphics PRIVATE SFML_DIRECTX_TESTS)
    target_include_directories(test-sfml-graphics PRIVATE ${PROJECT_SOURCE_DIR}/src)
    if(SFML_RUN_DISPLAY_TESTS)
        foreach(mode IN ITEMS directx-warp directx-min directx-high directx-unavailable)
            add_test(NAME sfml-graphics-${mode}
                     COMMAND test-sfml-graphics --sfml-${mode}
                     WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
            set_tests_properties(sfml-graphics-${mode} PROPERTIES RUN_SERIAL TRUE)
        endforeach()
        if(SFML_BUILD_OPENGL_BACKEND OR SFML_BUILD_VULKAN_BACKEND)
            add_test(NAME sfml-graphics-directx-fallback
                     COMMAND test-sfml-graphics --sfml-directx-fallback
                     WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
            set_tests_properties(sfml-graphics-directx-fallback PROPERTIES RUN_SERIAL TRUE)
        endif()
    endif()
endif()
if(SFML_RUN_DISPLAY_TESTS AND SFML_BUILD_OPENGL_BACKEND AND SFML_OS_WINDOWS)
    add_test(NAME sfml-graphics-opengl COMMAND test-sfml-graphics --sfml-opengl
             WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    set_tests_properties(sfml-graphics-opengl PROPERTIES RUN_SERIAL TRUE)
endif()
if(SFML_RUN_DISPLAY_TESTS AND SFML_BUILD_VULKAN_BACKEND AND SFML_OS_WINDOWS)
    add_test(NAME sfml-graphics-vulkan
             COMMAND test-sfml-graphics --sfml-vulkan
             WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    set_tests_properties(sfml-graphics-vulkan PROPERTIES SKIP_RETURN_CODE 77 RUN_SERIAL TRUE)
endif()
if(SFML_RUN_DISPLAY_TESTS)
    target_compile_definitions(test-sfml-graphics PRIVATE SFML_RUN_DISPLAY_TESTS)
endif()
set(GRAPHICS_TEST_TARGETS test-sfml-graphics)
if(SFML_OS_IOS AND SFML_BUILD_METAL_BACKEND AND SFML_BUILD_OPENGL_BACKEND)
    # Apps launched from the iOS home screen get no arguments, so each backend gets its own app
    sfml_add_test(test-sfml-graphics-gles "${GRAPHICS_SRC}" SFML::Graphics)
    target_compile_definitions(test-sfml-graphics PRIVATE SFML_TEST_DEFAULT_MODE="--sfml-metal")
    target_compile_definitions(test-sfml-graphics-gles PRIVATE SFML_GRAPHICS_TEST_MAIN SFML_TEST_DEFAULT_MODE="--sfml-opengl")
    if(SFML_RUN_DISPLAY_TESTS)
        target_compile_definitions(test-sfml-graphics-gles PRIVATE SFML_RUN_DISPLAY_TESTS)
    endif()
    list(APPEND GRAPHICS_TEST_TARGETS test-sfml-graphics-gles)
endif()

if (XCODE AND SFML_OS_IOS)
    set(GRAPHICS_RESOURCES
        invalid_shader.vert
        sfml-logo-big.bmp
        sfml-logo-big.gif
        sfml-logo-big.jpg
        sfml-logo-big.png
        sfml-logo-big.psd
        sfml-logo-big.qoi
        shader-🐌.vert
        shader-ń.vert
        shader-metal.metal
        shader.frag
        shader.geom
        shader.vert
        tuffy-🐌.ttf
        tuffy-ń.ttf
        tuffy.ttf
    )
    foreach(target IN LISTS GRAPHICS_TEST_TARGETS)
        target_sources(${target} PRIVATE ${GRAPHICS_RESOURCES})
    endforeach()
    set_source_files_properties(${GRAPHICS_RESOURCES} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    # Ship the MSL fixture as text instead of letting Xcode compile it into the app's default library
    set_source_files_properties(shader-metal.metal PROPERTIES XCODE_EXPLICIT_FILE_TYPE "text")
endif()
