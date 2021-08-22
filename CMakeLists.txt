cmake_minimum_required(VERSION 2.6)

include(cmake/CPM.cmake)
include(cmake/Dependencies.cmake)

project(king-of-the-bombspot)

set(info
    README.md
    TODO.md
    CHANGELOG.md
)

set(SRC_CLIENT
)

add_subdirectory(thirdparty/game-assets-0.1)

set(GAME_ASSETS_INCLUDE_DIR ${CMAKE_BINARY_DIR}/game-assets-prefix/src/game-assets/include)
set(GAME_ASSETS_LIBRARY ${CMAKE_BINARY_DIR}/game-assets-prefix/src/game-assets-build/libgame-assets.a)

find_package(OpenGL REQUIRED)

add_executable(king-of-the-bombspot
    src/common/application.h
    src/common/animatablecolor.cpp
    src/common/camera.cpp
    src/common/font.cpp
    src/common/glextl.cpp
    src/common/log.cpp
    src/common/main_sdl.cpp
    src/common/quickvertexbuffer.cpp
    src/common/settings.cpp
    src/common/shader.cpp
    src/common/stb_image.h
    src/common/stb_truetype.h
    src/filesystem.cpp
    src/program.cpp
    README.md
    TODO.md
    CHANGELOG.md
    )

target_include_directories(king-of-the-bombspot
    PUBLIC
       ${CMAKE_SOURCE_DIR}/src/common
)

target_link_libraries(king-of-the-bombspot
    game-assets
    SDL2-static
    glm
    ${OPENGL_LIBRARIES}
)
