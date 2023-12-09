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

include(FindPackageHandleStandardArgs)

find_library(
        LZ4_LIBRARY
        lz4
)

find_path(
        LZ4_INCLUDE_DIR
        lz4.h
)

if (EXISTS ${LZ4_INCLUDE_DIR})
    file(READ ${LZ4_INCLUDE_DIR}/lz4.h header)
    string(REGEX MATCH "LZ4_VERSION_MAJOR +([0-9]+)" _ ${header})
    set(major_version ${CMAKE_MATCH_1})
    string(REGEX MATCH "LZ4_VERSION_MINOR +([0-9]+)" _ ${header})
    set(minor_version ${CMAKE_MATCH_1})
    string(REGEX MATCH "LZ4_VERSION_RELEASE +([0-9]+)" _ ${header})
    set(patch_version ${CMAKE_MATCH_1})
    set(LZ4_VERSION ${major_version}.${minor_version}.${patch_version})
endif ()

set(LZ4_LIBRARIES ${LZ4_LIBRARY})
set(LZ4_INCLUDE_DIRS ${LZ4_INCLUDE_DIR})

find_package_handle_standard_args(
        LZ4
        REQUIRED_VARS
        LZ4_LIBRARIES
        LZ4_INCLUDE_DIRS
        VERSION_VAR LZ4_VERSION
)

