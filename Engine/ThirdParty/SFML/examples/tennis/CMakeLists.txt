# all source files
set(SRC Tennis.cpp)
if(SFML_OS_IOS)
    set(RESOURCES
        resources/ball.wav
        resources/sfml_logo.png
        resources/tuffy.ttf)
    set_source_files_properties(${RESOURCES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
endif()

# define the tennis target
sfml_add_example(tennis GUI_APP
                 SOURCES ${SRC}
                 BUNDLE_RESOURCES ${RESOURCES}
                 DEPENDS SFML::Audio SFML::Graphics)

if(SFML_OS_EMSCRIPTEN)
    set_target_properties(tennis PROPERTIES SUFFIX ".html")
    set_property(TARGET tennis APPEND PROPERTY LINK_DEPENDS "${PROJECT_SOURCE_DIR}/examples/web/shell.html")
    target_link_options(tennis PRIVATE
        -sEXIT_RUNTIME=1
        "--shell-file=${PROJECT_SOURCE_DIR}/examples/web/shell.html"
        "SHELL:--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/resources@resources")
endif()
