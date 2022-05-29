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

uniform sampler2D g_sBlendMap;
uniform sampler2D g_sColorMaps[3];
uniform sampler2D g_sNormalMaps[3];

in vec2 pass_UV0;
in vec2 pass_UV1;
in vec2 pass_UV2;
in vec2 pass_BlendUV;

out vec4 out_Color;

void main() {
	vec4 vColor0 = texture(g_sColorMaps[0], pass_UV0);
	vec4 vColor1 = texture(g_sColorMaps[1], pass_UV1);
	vec4 vColor2 = texture(g_sColorMaps[2], pass_UV2);
	vec2 vBlendFactor = texture(g_sBlendMap, pass_BlendUV).rg;

	float fBlendFactor1 = 1.0 - clamp((vColor1.a - vBlendFactor.r) * 32.0, 0.0, 1.0);
	float fBlendFactor2 = 1.0 - clamp((vColor2.a - vBlendFactor.g) * 32.0, 0.0, 1.0);

	vec3 vColor = fBlendFactor1 * (vColor1.rgb - vColor0.rgb) + vColor0.rgb;
	vColor = fBlendFactor2 * (vColor2.rgb - vColor.rgb) + vColor.rgb;

	out_Color = vec4(vColor.rgb, 1.0);
}
