# OpenAWE - A reimplementation of Remedy's Alan Wake Engine
#
# OpenAWE is the legal property of its developers, whose names
# can be found in the AUTHORS file distributed with this source
# distribution.
#
# OpenAWE is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 3
# of the License, or (at your option) any later version.
#
# OpenAWE is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with OpenAWE. If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.15)
project(OpenAWE VERSION 0.0.1)

include(TestBigEndian)
include(GoogleTest)
include(ExternalProject)
include(CTest)
include(GNUInstallDirs)

test_big_endian(BIG_ENDIAN)
if (BIG_ENDIAN)
    add_definitions(-DBIG_ENDIAN_SYSTEM)
else ()
    add_definitions(-DLITTLE_ENDIAN_SYSTEM)
endif ()

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# ------------------------------------
# Compiler flags
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

# ------------------------------------
# Third party libraries

# Mojoshader
file(
        GLOB_RECURSE

        MOJOSHADER_SOURCE_FILES

        3rdparty/mojoshader/*.c
        3rdparty/mojoshader/*.h
)
add_library(mojoshader ${MOJOSHADER_SOURCE_FILES})
target_include_directories(mojoshader PUBLIC 3rdparty/mojoshader)
set(MOJOSHADER_LIBRARIES mojoshader)
set(MOJOSHADER_INCLUDE_DIRS 3rdparty/mojoshader)

# ------------------------------------
# Dependencies
if(UNIX AND NOT OpenGL_GL_PREFERENCE)
    set(OpenGL_GL_PREFERENCE GLVND)
endif()

find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)
find_package(GLFW REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)
find_package(Bullet REQUIRED)
find_package(FMT REQUIRED)
find_package(OGG REQUIRED)
find_package(Theora REQUIRED)
find_package(Vorbis REQUIRED)
find_package(cxxopts REQUIRED)
find_package(GTest REQUIRED)
find_package(spdlog REQUIRED)
find_package(EnTT REQUIRED)
find_package(Boost REQUIRED)

find_package(Doxygen)
if (DOXYGEN_FOUND)
    doxygen_add_docs(
            doc
            ${PROJECT_SOURCE_DIR}
    )
endif ()

link_libraries(
        ${CMAKE_THREAD_LIBS_INIT}
        ${X11_LIBRARIES}
        ${ZLIB_LIBRARIES}
        ${GLFW_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${OPENGL_LIBRARIES}
        ${OPENAL_LIBRARY}
        ${LIBXML2_LIBRARIES}
        ${FMT_LIBRARIES}
        ${THEORA_LIBRARIES}
        ${VORBIS_LIBRARIES}
        ${OGG_LIBRARIES}
        ${BULLET_LIBRARIES}
        ${spdlog_LIBRARIES}
        ${MOJOSHADER_LIBRARIES}
)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/par

        ${ZLIB_INCLUDE_DIRS}
        ${GLFW_INCLUDE_DIRS}
        ${GLEW_INCLUDE_DIRS}
        ${OPENGL_INCLUDE_DIRS}
        ${LIBXML2_INCLUDE_DIRS}
        ${GLM_INCLUDE_DIRS}
        ${FMT_INCLUDE_DIRS}
        ${cxxopts_INCLUDE_DIRS}
        ${BULLET_INCLUDE_DIRS}
        ${spdlog_INCLUDE_DIRS}
        ${ENTT_INCLUDE_DIRS}
        ${MOJOSHADER_INCLUDE_DIRS}
)

#message(WARNING ${FBX_INCLUDE_DIRS})

add_definitions(-DGLM_ENABLE_EXPERIMENTAL)
add_definitions(-DSPDLOG_FMT_EXTERNAL)

# ------------------------------------
# Libraries for awe
file(GLOB_RECURSE SOURCE_FILES src/common/*.cpp src/common/*.h)
add_library(awe_common ${SOURCE_FILES})

file(GLOB_RECURSE SOURCE_FILES src/awe/*.cpp src/awe/*.h)
add_library(awe_lib ${SOURCE_FILES})
target_link_libraries(awe_lib awe_common)
set_target_properties(awe_lib PROPERTIES OUTPUT_NAME awe)

file(GLOB_RECURSE SOURCE_FILES src/graphics/*.cpp src/graphics/*.h)
add_library(awe_graphics ${SOURCE_FILES})
target_link_libraries(awe_graphics awe_common awe_lib)

file(GLOB_RECURSE SOURCE_FILES src/sound/*.cpp src/sound/*.h)
add_library(awe_sound ${SOURCE_FILES})
target_link_libraries(awe_sound awe_common awe_lib)

file(GLOB_RECURSE SOURCE_FILES src/physics/*.cpp src/physics/*.h)
add_library(awe_physics ${SOURCE_FILES})
target_link_libraries(awe_physics awe_common)

file(GLOB_RECURSE SOURCE_FILES src/video/*.cpp src/video/*.h)
add_library(awe_video ${SOURCE_FILES})
target_link_libraries(awe_video awe_common awe_lib awe_graphics)

file(GLOB_RECURSE SOURCE_FILES src/engines/*.cpp src/engines/*.h)
add_library(awe_engines ${SOURCE_FILES})
target_link_libraries(awe_engines awe_common awe_lib awe_graphics)

# ------------------------------------
# Main awe executable
file(GLOB SOURCE_FILES src/*.cpp src/*.h)
add_executable(awe ${SOURCE_FILES})
target_link_libraries(
        awe
        awe_common
        awe_lib
        awe_graphics
        awe_sound
        awe_physics
        awe_video
        awe_engines
)

install(
        TARGETS awe
        DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# ------------------------------------
# Unit Tests
list(FILTER SOURCE_FILES EXCLUDE REGEX \\.*/awe.cpp)
file(GLOB_RECURSE TEST_SOURCE_FILES test/*.cpp)
add_executable(awe_test ${TEST_SOURCE_FILES})
target_link_libraries(awe_test ${GTEST_BOTH_LIBRARIES} awe_common awe_lib)
gtest_add_tests(TARGET awe_test)
