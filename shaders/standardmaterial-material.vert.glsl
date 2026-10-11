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

#define MAX_BONES 64

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

uniform mat4x3 GPU_skinning_matrices[MAX_BONES];

in vec3 in_Position;
in vec2 in_UV0;
in vec4 in_Normal;
in ivec4 in_BoneID;
in vec4 in_BoneWeight;

out vec2 pass_UV;
out vec3 pass_ViewPosition;

void main() {
    mat4x3 mAnimationFactor = mat4x3(1.0f);
    mat4 mLocalToView = g_mWorldToView * g_mLocalToWorld;

#ifdef PROP_GLOB_SKINNED
    mAnimationFactor = mat4x3(0.0f);
    for (int i = 0; i < 4; i++) {
        mAnimationFactor += (in_BoneWeight[i] * (1.0/255.0)) * GPU_skinning_matrices[in_BoneID[i]];
    }
#endif // PROP_GLOB_SKINNED

    pass_ViewPosition = (mLocalToView * mat4(mAnimationFactor) * vec4(in_Position, 1.0)).xyz;

    pass_UV = in_UV0 * (1.0/4096.0);
    gl_Position = g_mViewToClip * vec4(pass_ViewPosition, 1.0);
}
