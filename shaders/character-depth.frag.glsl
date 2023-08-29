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

uniform sampler2D g_sColorMap;
uniform sampler2D g_sNormalMap;

in vec4 pass_ClipPosition;
in vec4 pass_ViewPosition;
in vec3 pass_Normal;
in vec3 pass_Tangent;
in vec3 pass_Bitangent;
in vec2 pass_UV;

layout(location=0) out vec4 out_Depth;
layout(location=1) out vec4 out_Normal;

void main() {
    out_Depth = vec4(pass_ClipPosition.zzz / pass_ClipPosition.w, 1.0f);

#ifdef PROP_NORMAL_MAP
    vec2 vNormalDx = texture(g_sNormalMap, pass_UV).wy * 2.0f - 1.0f;
    float fNormalFactor = sqrt(1.0 - dot(vNormalDx.x, vNormalDx.y));
    vec3 vNormal =
        mat3(pass_Tangent, pass_Bitangent, normalize(pass_Normal.xyz)) *
        vec3(vNormalDx, fNormalFactor);
#else // PROP_NORMAL_MAP
    vec3 vNormal = pass_Normal.xyz;
#endif // PROP_NORMAL_MAP

    out_Normal.xyz = vNormal * 0.5f + 0.5f;
    out_Normal.w = 1.0f;
}
