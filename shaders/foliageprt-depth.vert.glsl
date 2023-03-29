/* OpenAWE - A reimplementation of Remedys Alan Wake Engine
 *
 * OpenAWE is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * OpenAWE is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * OpenAWE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenAWE. If not, see <http://www.gnu.org/licenses/>.
 */

#version 330

uniform mat4 g_mLocalToView;
uniform mat4 g_mViewToClip;

in vec3 in_Position;
in vec4 in_Normal;
in vec2 in_UV0;

out vec2 pass_UV;

void main() {
    pass_UV = in_UV0 * (1.0/4096.0);
    gl_Position = g_mViewToClip * g_mLocalToView * vec4(in_Position, 1.0);
}
