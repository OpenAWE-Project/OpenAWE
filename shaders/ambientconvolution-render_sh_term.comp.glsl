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

#version 430

uniform vec3 g_vDirectionalSH[9];
uniform vec3 g_vAmbientIntensity;
uniform float g_fAmbientSaturation;

layout(binding=0, rgba16f) uniform readonly image2D g_sSource;
layout(binding=1, rgba16f) uniform writeonly image2D out_sTarget;

layout(local_size_x=9, local_size_y=1, local_size_z=1) in;

const vec3 vColorFactor = vec3(0.3086, 0.6094, 0.082);

void main() {
    const int iSH = int(gl_LocalInvocationID.x);
    const vec3 vDirectionalSH = g_vDirectionalSH[iSH];

    vec4 vColor = vec4(0.0);
    for (int i = 0; i < 6; i++) {
        vColor += imageLoad(g_sSource, ivec2(iSH, i));
    }

    const vec3 vColorIntensity = (vColor.rgb + vDirectionalSH) * g_vAmbientIntensity;
    const float vFactorizedColor = dot(vColorIntensity, vColorFactor);
    const vec3 vColor3 = (vColorIntensity - vFactorizedColor) * g_fAmbientSaturation + vFactorizedColor;

    imageStore(out_sTarget, ivec2(iSH, 0), vec4(vColor3, vColor.a));
}
