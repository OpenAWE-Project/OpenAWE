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

uniform sampler2D g_sColorMap;
uniform vec4 g_vColorMultiplier = vec4(1.0);

in vec2 pass_UV;

out vec4 out_Color;

void main() {
    vec4 vColor = texture(g_sColorMap, pass_UV);

    if (vColor.a < 0.5)
        discard;

    out_Color.rgb = vColor.rgb * g_vColorMultiplier.rgb;
    out_Color.a = 1.0;
}