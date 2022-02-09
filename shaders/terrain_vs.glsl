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
uniform vec4 g_vTexCoordScale1 = vec4(16.0);
uniform vec4 g_vTexCoordScale2 = vec4(16.0);

in vec3 in_Position;
in vec3 in_Normal;
in vec2 in_UV0;
in vec2 in_UV1;
in vec2 in_UV2;
in vec2 in_UV3;

out vec2 pass_UV0;
out vec2 pass_UV1;
out vec2 pass_UV2;
out vec2 pass_BlendUV;

void main() {
	vec4 vTexCoords1 = vec4(in_UV0, in_UV1) * g_vTexCoordScale1;
	vec4 vTexCoords2 = vec4(in_UV2, in_UV3) * g_vTexCoordScale2;
	pass_UV0 = vTexCoords1.xy;
	pass_UV1 = vTexCoords1.zw;
	pass_UV2 = vTexCoords2.xy;
	pass_BlendUV = vTexCoords2.zw;

	gl_Position = g_mViewToClip * g_mLocalToView * vec4(in_Position, 1.0);
}
