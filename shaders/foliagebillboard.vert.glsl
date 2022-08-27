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

struct FoliageMesh {
    vec2 vAtlasPosition;
    vec2 vAtlasSize;
    vec2 vBillboardSize;
};

struct FoliageInstance {
    vec3 vPosition;
    int iMeshID;
};

uniform FoliageMesh g_foliageMeshTable[48];

uniform mat4 g_mLocalToClip;
uniform mat4 g_mLocalToView;
uniform mat4 g_mViewToClip;

in vec3 in_Position;
in int in_Color0;

out vec2 pass_AtlasUV;

void main() {
    int iFoliageMesh = in_Color0;
    vec3 vPosition = in_Position;

    float fUpFactor = float(gl_VertexID % 4 == 1) + float(gl_VertexID % 4 == 2);
    float fSideFactor = -((gl_VertexID + 1) % 2) + gl_VertexID % 2;

    float fEvenFactor = (gl_VertexID + 1) % 2;

    FoliageMesh foliageMesh = g_foliageMeshTable[iFoliageMesh];

    vPosition.x += fSideFactor * foliageMesh.vBillboardSize.x / 2;
    vPosition = (g_mLocalToView * vec4(vPosition, 1.0)).xyz;
    vPosition.y += fUpFactor * foliageMesh.vBillboardSize.y;

    vec2 vUV = foliageMesh.vAtlasPosition;
    vUV.s += fEvenFactor * foliageMesh.vAtlasSize.s;
    vUV.t += (1 - fUpFactor) * foliageMesh.vAtlasSize.t;

    pass_AtlasUV = vUV;
    gl_Position = g_mViewToClip * vec4(vPosition, 1.0);
}
