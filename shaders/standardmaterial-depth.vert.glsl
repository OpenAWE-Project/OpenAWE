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

#version 330 core

uniform mat4 g_mLocalToView;
uniform mat4 g_mViewToClip;

in vec3 in_Position;
in vec4 in_Normal;

out vec4 pass_ClipPosition;
out vec4 pass_ViewPosition;
out vec3 pass_Normal;

void main() {
    pass_ViewPosition = g_mLocalToView * vec4(in_Position, 1.0);
    pass_ClipPosition = g_mViewToClip * pass_ViewPosition;
    pass_Normal = mat3(g_mLocalToView) * (in_Normal.xyz * (1.0/32767.0));
    gl_Position = pass_ClipPosition;
}
