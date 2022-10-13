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
#include "dds.h"

static const uint32_t DDS_MAGIC_ID = MKTAG('D', 'D', 'S', 0x20);

static const uint32_t DDS_DXT5 = MKTAG('D', 'X', 'T', '5');
static const uint32_t DDS_DXT3 = MKTAG('D', 'X', 'T', '3');
static const uint32_t DDS_DXT1 = MKTAG('D', 'X', 'T', '1');

namespace Graphics {

DDS::DDS(Common::ReadStream *dds) {
	uint32_t magicId = dds->readUint32BE();
	if (magicId != DDS_MAGIC_ID)
		throw std::runtime_error("Invalid dds file");

	uint32_t size = dds->readUint32LE();
	if (size != 124)
		throw std::runtime_error("Invalid dds header size");

	uint32_t flags = dds->readUint32BE();
	if (flags & 0x1)
		throw std::runtime_error("DDSD_CAPS required in dds file");

	uint32_t height = dds->readUint32LE();
	uint32_t width = dds->readUint32LE();

	uint32_t pitch = dds->readUint32LE();
	uint32_t depth = dds->readUint32LE();
	uint32_t mipMapCount = dds->readUint32LE();

	dds->skip(44);

	uint32_t pixelFormatSize = dds->readUint32LE();
	uint32_t pixelFormatFlags = dds->readUint32LE();
	uint32_t fourCC = dds->readUint32BE();

	switch (fourCC) {
		case DDS_DXT5:
			_format = kBC3;
			_compressed = true;
			break;
		case DDS_DXT3:
			_format = kBC2;
			_compressed = true;
			break;
		case DDS_DXT1:
			_format = kBC1;
			_compressed = true;
			break;
		default:
			throw std::runtime_error("Invalid pixel format");
	}

	uint32_t rgbBitCount = dds->readUint32LE();

	uint32_t redBitMask   = dds->readUint32LE();
	uint32_t greenBitMask = dds->readUint32LE();
	uint32_t blueBitMask  = dds->readUint32LE();
	uint32_t alphaBitMask = dds->readUint32LE();

	dds->skip(20);

	_layers.resize(mipMapCount);
	for (auto &mipMap: _layers) {
		mipMap.width = width;
		mipMap.height = height;

		size_t dataSize = std::max(16u, ((width + 3u) / 4u) * ((height + 3u) / 4u) * 16u);

		mipMap.dataSize = dataSize;
		mipMap.data.resize(1);
		mipMap.data[0] = new byte[dataSize];
		dds->read(mipMap.data[0], dataSize);

		width /= 2;
		height /= 2;
	}
}

}
