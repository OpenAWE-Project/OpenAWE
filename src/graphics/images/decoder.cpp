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

#include <cmath>
#include "decoder.h"

namespace Graphics {

ImageDecoder::ImageDecoder(TextureType type) : _format(kRGBA8), _type(type), _compressed(false) {
}

size_t ImageDecoder::getNumMipMaps() const {
	return _layers.size();
}

const ImageDecoder::Mipmap & ImageDecoder::getMipMap(unsigned int layer) const {
	return _layers[layer];
}

bool ImageDecoder::isCompressed() const {
	return _compressed;
}

TextureFormat ImageDecoder::getFormat() const {
	return _format;
}

TextureType ImageDecoder::getType() const {
	return _type;
}

size_t ImageDecoder::getImageSize(unsigned int width, unsigned int height, unsigned int depth) {
	switch (_format) {
		case kR8:
			return width * height * depth;

		case kRG8:
		case kA1RGB5:
			return width * height * depth * 2;

		case kRGB8:
			return width * height * depth * 3;

		case kR32F:
		case kRG16:
		case kRGBA8:
			return width * height * depth * 4;

		case kRGBA16F:
			return width * height * depth * 8;

		case kBC1:
			return std::max(8u, ((width + 3) / 4) * ((height + 3) / 4) * 8);

		case kBC2:
		case kBC3:
			return std::max(16u, ((width + 3) / 4) * ((height + 3) / 4) * 16);

		default:
			return 0;
	}
}

}
