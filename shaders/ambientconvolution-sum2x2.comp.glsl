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

layout(binding=0, rgba16f) uniform readonly image2D g_sSource;

layout(binding=1, rgba16f) uniform writeonly image2D out_sTarget;

layout(local_size_x=16, local_size_y=16, local_size_z=1) in;

void main() {
    const ivec2 vPosition = ivec2(gl_GlobalInvocationID.xy);

    const vec4 color0 = imageLoad(g_sSource, vPosition.xy * 2);
    const vec4 color1 = imageLoad(g_sSource, vPosition.xy * 2 + ivec2(1, 0));
    const vec4 color2 = imageLoad(g_sSource, vPosition.xy * 2 + ivec2(0, 1));
    const vec4 color3 = imageLoad(g_sSource, vPosition.xy * 2 + ivec2(1, 1));

    imageStore(out_sTarget, vPosition.xy, (color0 + color1 + color2 + color3));
}
