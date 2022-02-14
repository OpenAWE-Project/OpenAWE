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

#ifndef OPENAWE_GRAPHICS_TYPES_H
#define OPENAWE_GRAPHICS_TYPES_H

enum AttributeType {
	kPosition,
	kNormal,
	kBoneIndex,
	kBoneWeight,
	kColor,
	kDisplacementFactor,
	kTexCoord0,
	kTexCoord1,
	kTexCoord2,
	kTexCoord3,
};

enum TextureType {
	kTexture2D,
	kTexture3D,
	kTextureCube
};

enum TextureFormat {
	kR8,
	kRG8,
	kRG16,
	kRGB8,
	kRGBA8,
	kBC1,
	kBC2,
	kBC3
};

enum BufferType {
	kIndexBuffer,
	kVertexBuffer,
	kUniformBuffer
};

enum BlendMode {
	kNone,
	kMultiply
};

inline AttributeType getTexCoord(unsigned int index) {
	switch (index) {
		default:
		case 0: return kTexCoord0;
		case 1: return kTexCoord1;
		case 2: return kTexCoord2;
		case 3: return kTexCoord3;
	}
}

#endif //OPENAWE_TYPES_H
