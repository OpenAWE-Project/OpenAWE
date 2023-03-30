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

cmake_minimum_required(VERSION 3.18)
project(OpenAWE VERSION 0.0.1)

include(TestBigEndian)
include(GoogleTest)
include(ExternalProject)
include(CTest)
include(GNUInstallDirs)
include(CheckTypeSize)
include(CheckIncludeFile)
include(CheckFunctionExists)
include(CheckLinkerFlag)
include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)
include(CheckCSourceCompiles)
include(CheckCXXSourceCompiles)

test_big_endian(BIG_ENDIAN)
if (BIG_ENDIAN)
    add_definitions(-DBIG_ENDIAN_SYSTEM)
else ()
    add_definitions(-DLITTLE_ENDIAN_SYSTEM)
endif ()

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# ------------------------------------
# Options
option(USE_SYSTEM_CLI11 "Use the system CLI11" OFF)
option(USE_SYSTEM_ENTT "Use the system entt" OFF)

option(WITH_TOOLS "Compile with command line tool" ON)
option(WITH_VULKAN "Compile with support for vulkan" OFF)
option(WITH_COMPILED_SHADERS "Compile shader permutations during" ON)

# ------------------------------------
# Compiler flags
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

# ------------------------------------
# Compiler warnings
macro(check_and_set_compiler_flag_c flag)
    check_c_compiler_flag(${flag} hasC${flag})
    if (hasC${flag})
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${flag}")
    endif ()
endmacro()

macro(check_and_set_compiler_flag_cxx flag)
    check_cxx_compiler_flag(${flag} hasCXX${flag})
    if (hasCXX${flag})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
    endif ()
endmacro()

macro(check_and_set_compiler_flag flag)
    check_and_set_compiler_flag_c(${flag})
    check_and_set_compiler_flag_cxx(${flag})
endmacro()

check_and_set_compiler_flag("-Werror=vla")
check_and_set_compiler_flag("-Wswitch")
check_and_set_compiler_flag("-Wsign-compare")
check_and_set_compiler_flag_cxx("-Wreorder")

# ------------------------------------
# C++ checks
check_type_size(float FLOAT_SIZE)
if (NOT ${FLOAT_SIZE} EQUAL 4)
    message(FATAL_ERROR "Invalid size of float expected 4 got ${FLOAT_SIZE}")
endif ()

if(UNIX)
    check_include_file(pwd.h PWD_H)
    check_include_file(unistd.h UNISTD_H)
    if(NOT PWD_H)
        message(FATAL_ERROR "pwd.h is unavailable")
    endif()
    if(NOT UNISTD_H)
        message(FATAL_ERROR "unistd.h is unavailable")
    endif()
endif()

check_include_file(cpuid.h CPUID_H)
if(CPUID_H)
    add_definitions(-DHAS_CPUID_H)
endif()

# For some reason check_function_exists doesn't work on builtin gcc/clang functions
macro(check_builtin_function_exists function output)
    check_cxx_source_compiles("int main(int argc, char** argv) { __builtin_${function}; return 0; }" ${output})
endmacro()

check_linker_flag(CXX -Wl,--as-needed HAS_AS_NEEDED)
if (HAS_AS_NEEDED)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--as-needed")
endif ()

# GCC/Clang byte swap functions
check_builtin_function_exists(bswap16 HAS_BUILTIN_BSWAP16)
check_builtin_function_exists(bswap32 HAS_BUILTIN_BSWAP32)
check_builtin_function_exists(bswap64 HAS_BUILTIN_BSWAP64)

if (HAS_BUILTIN_BSWAP16)
    add_definitions(-DHAS_BUILTIN_BSWAP16)
endif ()
if (HAS_BUILTIN_BSWAP32)
    add_definitions(-DHAS_BUILTIN_BSWAP32)
endif ()
if (HAS_BUILTIN_BSWAP64)
    add_definitions(-DHAS_BUILTIN_BSWAP64)
endif ()

# MSVC byte swap functions
check_function_exists(_byteswap_ushort HAS_BYTESWAP_USHORT)
check_function_exists(_byteswap_ulong HAS_BYTESWAP_ULONG)
check_function_exists(_byteswap_uint64 HAS_BYTESWAP_UINT64)

if (HAS_BYTESWAP_USHORT)
    add_definitions(-DHAS_BYTESWAP_USHORT)
endif ()
if (HAS_BYTESWAP_ULONG)
    add_definitions(-DHAS_BYTESWAP_ULONG)
endif ()
if (HAS_BYTESWAP_UINT64)
    add_definitions(-DHAS_BYTESWAP_UINT64)
endif ()

# MSVC needs _USE_MATH_DEFINES defined to use c++ math constants like M_PI
if (MSVC)
    add_definitions(-D_USE_MATH_DEFINES)
endif ()

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
add_definitions(-DMOJOSHADER_NO_VERSION_INCLUDE)
add_definitions(-DSUPPORT_PROFILE_HLSL=0)
add_definitions(-DSUPPORT_PROFILE_SPIRV=0)
add_definitions(-DSUPPORT_PROFILE_GLSPIRV=0)

# ------------------------------------
# Dependencies

# PkgConfig is used as an alternative if packages are not found.
find_package(PkgConfig)

find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(OpenAL REQUIRED)
find_package(Bullet REQUIRED)
find_package(fmt REQUIRED)
find_package(Ogg REQUIRED)
find_package(Theora REQUIRED)
find_package(Vorbis REQUIRED)
find_package(spdlog REQUIRED)

if (WITH_VULKAN)
    # Vulkan is necessary since the graphics part has some stubs for future vulkan support, even if it doesn't work at the
    # moment. For this reason the awe executable is not linked to vulkan currently
    find_package(Vulkan REQUIRED)
    add_definitions(-DWITH_VULKAN)
endif ()

find_package(GTest)

if(NOT USE_SYSTEM_CLI11)
    add_subdirectory(3rdparty/CLI11)
else()
    find_package(CLI11 REQUIRED)
endif()

if(NOT USE_SYSTEM_ENTT)
    set(ENTT_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/3rdparty/entt/src)
else()
    find_package(EnTT REQUIRED)
endif()

find_package(Doxygen)
if (DOXYGEN_FOUND)
    doxygen_add_docs(
            doc
            ${PROJECT_SOURCE_DIR}/src
    )
endif ()

# Github actions machines do not have a version of tinyxml2 which has the cmake module files, so if pkg config is
# available it is tried to use it to retrieve the tinyxml2 libraries and include dirs through the package config (pc)
# file if the package search fails.
if(PKG_CONFIG_FOUND)
    find_package(tinyxml2)
    if (NOT tinyxml2_FOUND)
        pkg_search_module(TINYXML2 REQUIRED tinyxml2)

        add_library(tinyxml2::tinyxml2 SHARED IMPORTED)
        set_target_properties(
                tinyxml2::tinyxml2
                PROPERTIES
                IMPORTED_LOCATION ${TINYXML2_LINK_LIBRARIES}
        )
    endif ()
# If pkgconfig is not available the package search stays as required
else()
    find_package(tinyxml2 REQUIRED)
endif ()

link_libraries(
        ${CMAKE_THREAD_LIBS_INIT}

        $<TARGET_NAME_IF_EXISTS:glm::glm>
        $<TARGET_NAME_IF_EXISTS:glm>

        fmt::fmt
        CLI11::CLI11

        ${spdlog_LIBRARIES}
)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}

        ${ZLIB_INCLUDE_DIRS}
        ${OPENGL_INCLUDE_DIRS}
        ${OPENAL_INCLUDE_DIR}
        ${BULLET_INCLUDE_DIRS}
        ${spdlog_INCLUDE_DIRS}
        ${ENTT_INCLUDE_DIRS}
        ${Vulkan_INCLUDE_DIRS}
        ${MOJOSHADER_INCLUDE_DIRS}
)

add_definitions(-DGLM_ENABLE_EXPERIMENTAL)
add_definitions(-DSPDLOG_FMT_EXTERNAL)

# ------------------------------------
# Libraries for awe
file(GLOB_RECURSE SOURCE_FILES src/common/*.cpp src/common/*.h)
add_library(awe_common ${SOURCE_FILES})
target_link_libraries(
        awe_common

        tinyxml2::tinyxml2
        ${ZLIB_LIBRARIES}
)

file(GLOB_RECURSE SOURCE_FILES src/codecs/*.cpp src/codecs/*.h)
add_library(awe_codecs ${SOURCE_FILES})
target_link_libraries(
        awe_codecs

        awe_common

        ${THEORA_LIBRARIES}
        ${VORBIS_LIBRARIES}
        ${OGG_LIBRARIES}
)

file(GLOB_RECURSE SOURCE_FILES src/awe/*.cpp src/awe/*.h)
add_library(awe_lib ${SOURCE_FILES})
target_link_libraries(awe_lib awe_common)
if (NOT WIN32)
    # MSVC doesn't like to have libraries and executables with the same identifier
    set_target_properties(awe_lib PROPERTIES OUTPUT_NAME awe)
endif ()

file(GLOB_RECURSE SOURCE_FILES src/platform/*.cpp src/platform/*.h)
add_library(awe_platform ${SOURCE_FILES})
target_link_libraries(
        awe_platform

        awe_common

        glfw
)

file(GLOB_RECURSE SOURCE_FILES src/events/*.cpp src/events/*.h)
add_library(awe_events ${SOURCE_FILES})
target_link_libraries(awe_events awe_common)

file(GLOB_RECURSE SOURCE_FILES src/graphics/*.cpp src/graphics/*.h)
add_library(awe_graphics ${SOURCE_FILES})
target_link_libraries(
        awe_graphics

        awe_common
        awe_lib
        awe_platform

        GLEW::GLEW
        OpenGL::GL
        ${MOJOSHADER_LIBRARIES}
)

file(GLOB_RECURSE SOURCE_FILES src/sound/*.cpp src/sound/*.h)
add_library(awe_sound ${SOURCE_FILES})
target_link_libraries(
        awe_sound

        awe_common
        awe_lib
        awe_codecs

        ${OPENAL_LIBRARY}
)

file(GLOB_RECURSE SOURCE_FILES src/physics/*.cpp src/physics/*.h)
add_library(awe_physics ${SOURCE_FILES})
target_link_libraries(
        awe_physics

        awe_common

        ${BULLET_LIBRARIES}
)

file(GLOB_RECURSE SOURCE_FILES src/video/*.cpp src/video/*.h)
add_library(awe_video ${SOURCE_FILES})
target_link_libraries(awe_video awe_common awe_lib awe_graphics awe_sound)

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
        awe_platform
        awe_events
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
# Shaders
if (WITH_COMPILED_SHADERS)
    include(CompileTechnique)
    find_package(GLSLC REQUIRED)
    add_custom_target(awe_shaders)
    file(GLOB TECHNIQUE_FILES "techniques/*.json")
    foreach(technique_file IN ITEMS ${TECHNIQUE_FILES})
        compile_technique(awe_shaders ${GLSLC_EXECUTABLE} ${technique_file})
    endforeach()
    add_dependencies(awe awe_shaders)
endif ()

# ------------------------------------
# Tools
if (WITH_TOOLS)
    add_subdirectory(tools)
endif ()

# ------------------------------------
# Unit Tests
if (GTEST_FOUND)
    list(FILTER SOURCE_FILES EXCLUDE REGEX \\.*/awe.cpp)
    file(GLOB_RECURSE TEST_SOURCE_FILES test/*.cpp)
    add_executable(awe_test ${TEST_SOURCE_FILES})
    target_link_libraries(
            awe_test

            ${GTEST_BOTH_LIBRARIES}
            ${ZLIB_LIBRARIES}
            tinyxml2::tinyxml2

            awe_common
            awe_lib
    )
    gtest_add_tests(TARGET awe_test)
endif ()

# ------------------------------------
# Packaging
set(CPACK_GENERATOR "ZIP")
include(CPack)
