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

layout(std140) uniform model {
	mat4 g_mLocalToWorld;
	mat4 g_mWorldToLocal;
};

layout(std140) uniform material {
	vec4 g_vTexCoordScale1;
	vec4 g_vTexCoordScale2;
};

in vec3 in_Position;
in vec3 in_Normal;
in vec4 in_UV0;
in vec4 in_UV1;

out vec2 pass_UV0;
out vec2 pass_UV1;
out vec2 pass_UV2;
out vec2 pass_BlendUV;

void main() {
	vec4 vTexCoords1 = in_UV0 * g_vTexCoordScale1;
	vec4 vTexCoords2 = in_UV1 * g_vTexCoordScale2;
	pass_UV0 = vTexCoords1.xy;
	pass_UV1 = vTexCoords1.zw;
	pass_UV2 = vTexCoords2.xy;
	pass_BlendUV = vTexCoords2.zw;

	gl_Position = g_mWorldToClip * g_mLocalToWorld * vec4(in_Position, 1.0);
}
