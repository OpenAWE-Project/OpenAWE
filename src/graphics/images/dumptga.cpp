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

#include "src/graphics/images/dumptga.h"

namespace Graphics {

void dumpTGA(Common::WriteStream &tga, ImageDecoder &imageDecoder) {
	ImageDecoder::Mipmap mipmap = imageDecoder.getMipmaps(0)[0];

	tga.writeByte(0); // Image ID Length
	tga.writeByte(0); // Color Map Type
	tga.writeByte(2); // Image Type

	// Color Map Specification
	tga.writeUint16LE(0); // First Entry Index
	tga.writeUint16LE(0); // Color Map Length
	tga.writeByte(0); // Color Map Entry Size

	// Image specification
	tga.writeUint16LE(0); // X-Origin
	tga.writeUint16LE(0); // Y-Origin
	tga.writeUint16LE(mipmap.width); // Image width
	tga.writeUint16LE(mipmap.height); // Image height
	tga.writeByte(24); // Pixel Depth
	tga.writeByte(0); // Image Descriptor

	// Image data
	tga.write(mipmap.data[0], mipmap.dataSize);
}

}
