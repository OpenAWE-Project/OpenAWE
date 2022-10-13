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

#ifndef AWE_DECODER_H
#define AWE_DECODER_H

#include <vector>

#include "src/common/types.h"

#include "src/graphics/types.h"

namespace Graphics {

class ImageDecoder : public Common::Noncopyable {
public:
	struct Mipmap {
		std::vector<byte *> data;
		unsigned int dataSize, width, height, depth;
	};

	ImageDecoder();
	virtual ~ImageDecoder();

	size_t getNumMipMaps() const;
	[[nodiscard]] const Mipmap & getMipMap(unsigned int layer = 0) const;
	[[nodiscard]] bool isCompressed() const;
	TextureFormat getFormat() const;
	TextureType getType() const;

protected:
	size_t getImageSize(unsigned int width, unsigned int height);

	std::vector<Mipmap> _layers;
	TextureFormat _format;
	TextureType _type;
	bool _compressed;
};

} // End of namespace Graphics

#endif //AWE_DECODER_H
