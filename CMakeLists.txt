cmake_minimum_required(VERSION 2.6)

project(king-of-the-bombspot)

set(info
    README.md
    TODO.md
    CHANGELOG.md
)

set(SRC_CLIENT
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
)

if(${CMAKE_BUILD_TYPE} MATCHES DEBUG)
    message(STATUS "CMAKE_BUILD_TYPE: Debug")
else()
    message(STATUS "CMAKE_BUILD_TYPE: Release")
endif(${CMAKE_BUILD_TYPE} MATCHES DEBUG)

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH} "${CMAKE_PREFIX_PATH}/cmake-modules/"
)

if(CMAKE_COMPILER_IS_GNUCXX)
   SET(ENABLE_CXX11 "-std=c++11")

   EXECUTE_PROCESS(COMMAND "${CMAKE_CXX_COMPILER} -dumpversion" OUTPUT_VARIABLE GCC_VERSION)
   if (GCC_VERSION VERSION_LESS 4.7)
	  SET(ENABLE_CXX11 "-std=c++0x")
   endif()

   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ENABLE_CXX11}")
endif()

message(STATUS "CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}")

include(ExternalProject)
ExternalProject_Add(game-assets
	GIT_REPOSITORY https://github.com/wtrsltnk/game-assets.git
	CMAKE_ARGS -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH} -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
	INSTALL_COMMAND ${CMAKE_COMMAND} --version
	)

set(GAME_ASSETS_INCLUDE_DIR ${CMAKE_BINARY_DIR}/game-assets-prefix/src/game-assets/include)
set(GAME_ASSETS_LIBRARY ${CMAKE_BINARY_DIR}/game-assets-prefix/src/game-assets-build/libgame-assets.a)

include_directories(${GAME_ASSETS_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src/common)

find_package(OpenGL REQUIRED)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

find_package(GLM REQUIRED)
include_directories(${GLM_INCLUDE_DIR})

set(KOTB king-of-the-bombspot)
add_executable(${KOTB} ${SRC_CLIENT} ${info})

target_link_libraries(${KOTB}
	${GAME_ASSETS_LIBRARY}
	${SDL2_LIBRARY}
	${OPENGL_LIBRARIES}
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries(${KOTB} ${X11_LIBRARY})
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
