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

uniform float g_fSkyBrightness;
uniform float g_fSkySaturation;
uniform sampler2D g_sDistortMap;
uniform sampler3D g_sNoiseMap;
uniform sampler3D g_sSkyLUT;
uniform vec3 g_vSunDir;
uniform vec4 g_vViewPoint;

in vec3 pass_Position;
in vec2 pass_Direction;
in vec2 pass_UV;

out vec4 out_Color;

void main() {
    vec3 vNormPosition = normalize(pass_Position);
    vec3 vSkyLUTCoord = vec3(0);

    // X is the angle to the sun
    vSkyLUTCoord.x = dot(normalize(g_vSunDir.xz), normalize(vNormPosition.xz));

    // Y is the actual height in the sky sphere
    vSkyLUTCoord.y = sign(vNormPosition.y) * sqrt(abs(vNormPosition.y));

    // Z is the current height of the sun which influences more or less the time of day
    vSkyLUTCoord.z = sign(g_vSunDir.y) * sqrt(abs(g_vSunDir.y));

    vSkyLUTCoord = vSkyLUTCoord * 0.5 + 0.5;
    vec3 vSkyValue = texture(g_sSkyLUT, vSkyLUTCoord).rgb;

    out_Color = vec4(50 * vSkyValue * g_fSkyBrightness, 1.0);
}
