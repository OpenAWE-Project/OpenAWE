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

layout(binding=0, r32f) uniform readonly image2D g_sBasisFunc;
layout(binding=1, rgba16f) uniform readonly image2D g_sSource;

layout(binding=2, rgba16f) uniform writeonly image2D out_sTarget;

layout(local_size_x=16, local_size_y=16, local_size_z=1) in;

void main() {
    ivec2 vPosition = ivec2(gl_GlobalInvocationID.xy);
    ivec2 vLocalPosition = ivec2(gl_LocalInvocationID.xy);

    vec4 vSource = imageLoad(g_sSource, vLocalPosition);
    float vBasis = imageLoad(g_sBasisFunc, vPosition).r;

    imageStore(out_sTarget, vPosition, vBasis * vSource);
}
