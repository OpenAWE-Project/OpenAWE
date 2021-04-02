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

find_path(
        GLM_INCLUDE_DIR
        glm/glm.hpp
)

set(GLM_LIBRARIES ${GLM_LIBRARY})
set(GLM_INCLUDE_DIRS ${GLM_INCLUDE_DIR})

find_package_handle_standard_args(
        GLM
        REQUIRED_VARS
        GLM_INCLUDE_DIRS
)