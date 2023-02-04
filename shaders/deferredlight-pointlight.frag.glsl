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

uniform mat4 g_mClipToView;
uniform vec2 g_vScreenRes;
uniform vec3 g_vLightPosition;
uniform vec3 g_vLightColor;

uniform sampler2D g_sLinearDepthBuffer;
uniform sampler2D g_sNormalBuffer;

out vec4 out_Color;
out vec4 out_Normal;

void main() {
    vec2 vScreen = gl_FragCoord.xy / g_vScreenRes;

    float fDecay = g_vLightFalloff.x;
    float fDepth = texture(g_sLinearDepthBuffer, vScreen).r;
    vec3 vNormal = normalize(texture(g_sNormalBuffer, vScreen).xyz * 2.0 - 1.0);

    vec4 vViewPositionRel = g_mClipToView * vec4(vScreen * 2.0 - 1.0, fDepth, 1.0);
    vec3 vViewPosition = (vViewPositionRel.xyz / vViewPositionRel.w);

    // Calculate normal factor
    vec3 vDirection = normalize(g_vLightPosition - vViewPosition);
    float fNormalFactor = clamp(dot(vDirection, vNormal), 0.0, 1.0);

    // Calculate distance factor
    float fDistance = distance(g_vLightPosition, vViewPosition);
    float fDistanceFactor = clamp(1.0 - fDistance, 0.0, 1.0);

    out_Color.rgb = g_vLightColor;
    out_Color.a = fDistanceFactor * fNormalFactor;

    out_Normal.rgb = texture(g_sNormalBuffer, vScreen).xyz;
    out_Normal.a = 1.0;
}
