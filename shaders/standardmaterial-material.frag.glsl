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

#define PI 3.14159265358979323846264338327950288419716939937510

struct Atmosphere {
    vec3 vSunDir;
    vec3 vSunE;
    vec3 vExtinction;
    vec3 vRayleigh;
    vec3 vMie;
    vec3 vHGConstants;
    vec2 vFog;
};

layout(std140) uniform global {
    mat4 g_mWorldToView;
    mat4 g_mWorldToClip;
    mat4 g_mViewToWorld;
    mat4 g_mViewToClip;
    mat4 g_mClipToWorld;
    mat4 g_mClipToView;
    vec2 g_vScreenRes;
    Atmosphere g_atmosphere;
};

// Sunlight
uniform struct {
    vec4 vIntensity;
    vec3 vDir;
    vec4 vRegionStart;
    vec4 vRegionEnd;
} g_sunlight;

uniform sampler2D g_sColorMap;
uniform sampler2D g_sLightBuffer;

uniform vec4 g_vColorMultiplier = vec4(1.0);

in vec2 pass_UV;
in vec3 pass_ViewPosition;

out vec4 out_Color;
out float out_Depth;

void main() {
    vec2 vScreen = gl_FragCoord.xy / g_vScreenRes;

    vec4 vColor = texture(g_sColorMap, pass_UV) * g_vColorMultiplier;

    if (vColor.a < 0.5)
        discard;

    vec4 vLight = texture(g_sLightBuffer, vScreen);

    float fPhase = dot(-g_atmosphere.vSunDir, normalize(-pass_ViewPosition));

    float fMiePhase = g_atmosphere.vHGConstants.x / pow((g_atmosphere.vHGConstants.z * fPhase) + g_atmosphere.vHGConstants.y, 1.5);
    float fRayleighPhase = (pow(fPhase, 2.0) + 1.0) * (3.0/16.0) * PI;

    out_Color.rgb = mix(vColor.rgb, vLight.rgb, min(vLight.a, 1.0f));
    out_Color.rgb += g_atmosphere.vSunE * (fRayleighPhase * g_atmosphere.vRayleigh + fMiePhase * g_atmosphere.vMie);

    out_Color.a = 1.0;
}
