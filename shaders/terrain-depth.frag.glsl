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

uniform sampler2D g_sColorMaps[3];
uniform sampler2D g_sNormalMaps[3];
uniform sampler2D g_sBlendMap;
uniform sampler2D g_sGeoNormalMap;

uniform mat4 g_mWorldToView;

in vec4 pass_ClipPosition;
in vec4 pass_ViewPosition;
in vec4 pass_Normal;
in vec3 pass_Tangent0;
in vec3 pass_Tangent1;
in vec3 pass_Tangent2;
in vec2 pass_UV0;
in vec2 pass_UV1;
in vec2 pass_UV2;
in vec2 pass_BlendUV;

layout(location=0) out vec4 out_Depth;
layout(location=1) out vec4 out_Normal;

void main() {
    out_Depth = vec4(pass_ClipPosition.zzz / pass_ClipPosition.w, 1.0);

    vec3 vGeoNormal = texture(g_sGeoNormalMap, pass_BlendUV).xyz * 2.0 - 1.0;
    vec3 vBitangent0 = cross(vGeoNormal, pass_Tangent0);
    vec3 vBitangent1 = cross(vGeoNormal, pass_Tangent1);
    vec3 vBitangent2 = cross(vGeoNormal, pass_Tangent2);

    vec4 vColor1 = texture(g_sColorMaps[1], pass_UV1);
    vec4 vColor2 = texture(g_sColorMaps[2], pass_UV2);

    vec2 vBlendFactor = texture(g_sBlendMap, pass_BlendUV).rg;

    float fBlendFactor1 = 1.0 - clamp((vColor1.a - vBlendFactor.r) * 32.0, 0.0, 1.0);
    float fBlendFactor2 = 1.0 - clamp((vColor2.a - vBlendFactor.g) * 32.0, 0.0, 1.0);

    vec3 vBitangent = mix(vBitangent0, vBitangent1, fBlendFactor1);
    vBitangent      = mix(vBitangent,  vBitangent2, fBlendFactor2);

    vec3 vTangent = mix(pass_Tangent0, pass_Tangent1, fBlendFactor1);
    vTangent      = mix(vTangent,      pass_Tangent2, fBlendFactor2);

    vec2 vNormalDx0 = texture(g_sNormalMaps[0], pass_UV0).wy;
    vec2 vNormalDx1 = texture(g_sNormalMaps[1], pass_UV1).wy;
    vec2 vNormalDx2 = texture(g_sNormalMaps[2], pass_UV2).wy;

    vec2 vNormalDx = mix(vNormalDx0, vNormalDx1, fBlendFactor1);
    vNormalDx      = mix(vNormalDx,  vNormalDx2, fBlendFactor2);

    vNormalDx = vNormalDx * 2.0 - 1.0;

    float fNormalZ = sqrt(1.0 - vNormalDx.x * vNormalDx.y);

    vec3 vNormal =
        mat3(vTangent, vBitangent, normalize(vGeoNormal.xyz)) *
        vec3(vNormalDx, fNormalZ);

    out_Normal = vec4((mat3(g_mWorldToView) * normalize(vNormal)) * 0.5 + 0.5, 1.0);
}
