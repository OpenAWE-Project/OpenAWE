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
	vec4 vBlendFactor = vec4(texture(g_sBlendMap, pass_BlendUV).rg, 0.0, 0.0);

	vColor0 = vec4(mix(vColor0.rgb, vColor1.rgb, (1.0 - vColor1.a)), 1.0);
	vColor0 = vec4(mix(vColor0.rgb, vColor2.rgb, (1.0 - vColor2.a)), 1.0);

	out_Color = vec4(vColor0.rgb, 1.0);
}
