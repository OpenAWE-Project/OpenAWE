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
uniform vec4 g_vTexCoordScale1;
uniform vec4 g_vTexCoordScale2;

in vec3 in_Position;
in vec3 in_Tangent0;
in vec3 in_Tangent1;
in vec3 in_Tangent2;
in vec4 in_Normal;
in vec4 in_UV0;
in vec4 in_UV1;

out vec4 pass_ClipPosition;
out vec4 pass_ViewPosition;
out vec4 pass_Normal;
out vec3 pass_Tangent0;
out vec3 pass_Tangent1;
out vec3 pass_Tangent2;
out vec2 pass_UV0;
out vec2 pass_UV1;
out vec2 pass_UV2;
out vec2 pass_BlendUV;

void main() {
    vec4 vTexCoords1 = in_UV0 * g_vTexCoordScale1;
    vec4 vTexCoords2 = in_UV1 * g_vTexCoordScale2;
    pass_UV0 = vTexCoords1.xy;
    pass_UV1 = vTexCoords1.zw;
    pass_UV2 = vTexCoords2.xy;
    pass_BlendUV = vTexCoords2.zw;

    pass_Normal.xyz = mat3(g_mLocalToView) * in_Normal.xyz;
    pass_Normal.w = 1.0;
    pass_Tangent0 = 2.0 * in_Tangent0 - 1.0;
    pass_Tangent1 = 2.0 * in_Tangent1 - 1.0;
    pass_Tangent2 = 2.0 * in_Tangent2 - 1.0;

    pass_ViewPosition = g_mLocalToView * vec4(in_Position, 1.0);
    pass_ClipPosition = g_mViewToClip * pass_ViewPosition;
    gl_Position = pass_ClipPosition;
}
