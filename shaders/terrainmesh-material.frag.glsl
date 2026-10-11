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
uniform sampler2D g_sColorMap;
uniform sampler2D g_sNormalMap;

in vec2 pass_UV;

out vec4 out_Color;

void main() {
    out_Color = texture(g_sColorMap, pass_UV);
}
