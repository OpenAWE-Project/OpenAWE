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

#include "src/common/exception.h"

#include "src/graphics/images/dumpdds.h"

static const uint32_t kDDS = MKTAG('D', 'D', 'S', ' ');

static const uint32_t kEmpty = MKTAG('\0', '\0', '\0', '\0');
static const uint32_t kDXT1 = MKTAG('D', 'X', 'T', '1');
static const uint32_t kDXT3 = MKTAG('D', 'X', 'T', '3');
static const uint32_t kDXT5 = MKTAG('D', 'X', 'T', '5');

enum DDSFlags {
	kCaps        = 0x1,
	kHeight      = 0x2,
	kWidth       = 0x4,
	kPitch       = 0x8,
	kPixelFormat = 0x1000,
	kMipMapCount = 0x20000,
	kLinearSize  = 0x80000,
	kDepth       = 0x800000
};

enum DDSCaps {
	kComplex = 0x8,
	kTexture = 0x1000,
	kMipmap  = 0x400000
};

enum PixelFormatFlags {
	kAlphaPixels = 0x1,
	kAlpha       = 0x2,
	kFourCC      = 0x4,
	kRGB         = 0x40,
	kYUV         = 0x200,
	kLuminance   = 0x20000
};

namespace Graphics {

void dumpDDS(Common::WriteStream &dds, ImageDecoder &imageDecoder) {
	dds.writeUint32BE(kDDS); // Magic header
	dds.writeUint32LE(124);  // Header size

	uint32_t flags = kCaps | kWidth | kHeight | kPixelFormat;
	uint32_t caps = kTexture;

	if (imageDecoder.getNumMipMaps() > 1) {
		flags |= kMipMapCount;
		caps |= kMipmap;
	}

	dds.writeUint32LE(flags);

	dds.writeUint32LE(imageDecoder.getMipMap().height);
	dds.writeUint32LE(imageDecoder.getMipMap().width);

	dds.writeUint32LE(0); // Pitch
	dds.writeUint32LE(0); // Depth
	dds.writeUint32LE(imageDecoder.getNumMipMaps()); // Mipmap count

	// Reserved space
	dds.writeZeros(44);

	// Pixel Format
	//
	dds.writeUint32LE(32);

	switch (imageDecoder.getFormat()) {
		case kA1RGB5:
			dds.writeUint32LE(kAlphaPixels | kRGB);
			dds.writeUint32LE(kEmpty);
			dds.writeUint32LE(16);
			dds.writeUint32LE(0x7c00);
			dds.writeUint32LE(0x03e0);
			dds.writeUint32LE(0x001f);
			dds.writeUint32LE(0x8000);
			break;

		case kBC1:
			dds.writeUint32LE(kFourCC);
			dds.writeUint32BE(kDXT1);
			dds.writeUint32LE(0);
			dds.writeUint32LE(0);
			dds.writeUint32LE(0);
			dds.writeUint32LE(0);
			dds.writeUint32LE(0);
			break;

		case kBC2:
			dds.writeUint32LE(kFourCC);
			dds.writeUint32BE(kDXT3);
			dds.writeUint32LE(0);
			dds.writeUint32LE(0);
			dds.writeUint32LE(0);
			dds.writeUint32LE(0);
			dds.writeUint32LE(0);
			break;

		case kBC3:
			dds.writeUint32LE(kFourCC);
			dds.writeUint32BE(kDXT5);
			dds.writeUint32LE(0);
			dds.writeUint32LE(0);
			dds.writeUint32LE(0);
			dds.writeUint32LE(0);
			dds.writeUint32LE(0);
			break;

		default:
			throw CreateException("TODO: Implement image format {}", fmt::underlying(imageDecoder.getFormat()));
	}

	dds.writeUint32LE(caps);
	dds.writeUint32LE(0); // dwCaps2
	dds.writeUint32LE(0); // dwCaps3
	dds.writeUint32LE(0); // dwCaps4
	dds.writeUint32LE(0); // reserved

	for (size_t i = 0; i < imageDecoder.getNumMipMaps(); ++i) {
		dds.write(imageDecoder.getMipMap(i).data[0], imageDecoder.getMipMap(i).dataSize);
	}
}

} // End of namespace Graphics
