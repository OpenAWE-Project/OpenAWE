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

#include <stdexcept>
#include <assert.h>
#include "tex.h"

namespace Graphics {

TEX::TEX(Common::ReadStream &tex) {
	const uint32_t type = tex.readUint32LE();
	const uint32_t format = tex.readUint32LE();
	uint32_t width = tex.readUint32LE();
	uint32_t height = tex.readUint32LE();
	uint32_t depth = tex.readUint32LE();
	const uint32_t mipmapCount = tex.readUint32LE();
	const uint32_t filter = tex.readUint32LE();

	//assert(filter == 0);

	tex.seek(0x20);

	switch (type) {
		case 0: // Texture2D
			_type = kTexture2D;
			break;

		case 1: // Texture3D
			_type = kTexture3D;
			break;

		case 2: // Cubemap
			_type = kTextureCube;
			break;

		default:
			throw std::runtime_error("Invalid texture type");
	}

	switch (format) {
		// RGBA8 LUT
		// RGBA8
		case 4:
		case 8:
			_format = kRGBA8;
			break;

		// RGBA16F
		case 6:
		case 10:
			_format = kRGBA16F;
			break;

		// DXT1
		case 1:
		case 5:
			_format = kBC1;
			_compressed = true;
			break;

		// DXT5
		case 3:
		case 7:
		case 9:
			_format = kBC3;
			_compressed = true;
			break;

		default:
			throw std::runtime_error("Invalid texture format");
	}

	_layers.resize(1);
	std::vector<Mipmap> &mipmaps = _layers;
	mipmaps.resize(mipmapCount);
	for (auto &mipmap : mipmaps) {
		const unsigned int imageSize = getImageSize(width, height, depth);

		mipmap.width = width;
		mipmap.height = height;
		mipmap.depth = depth;

		if (type == 2)
			mipmap.data.resize(6);
		else
			mipmap.data.resize(1);

		for (auto &datum : mipmap.data) {
			datum.resize(imageSize);
			tex.read(datum.data(), imageSize);
		}


		width /= 2;
		height /= 2;
		depth /= 2;

		width = std::max(width, 1u);
		height = std::max(height, 1u);
		depth = std::max(depth, 1u);
	}
}

}
