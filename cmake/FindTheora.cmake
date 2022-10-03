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
        THEORA_LIBRARY
        theoradec # Since we are only decoding theora prefer the decoding library over the full library
        theora
)

find_path(
        THEORA_INCLUDE_DIR
        theora/theora.h
)

set(THEORA_LIBRARIES ${THEORA_DECODING_LIBRARY})
set(THEORA_INCLUDE_DIRS ${THEORA_INCLUDE_DIR})

find_package_handle_standard_args(
        Theora
        REQUIRED_VARS
        THEORA_LIBRARIES
        THEORA_INCLUDE_DIRS
)