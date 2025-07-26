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

#ifndef AWE_SURFACE_H
#define AWE_SURFACE_H

#include "src/common/types.h"

#include "src/graphics/images/decoder.h"

namespace Graphics {

class Surface;

typedef std::shared_ptr<Surface> SurfacePtr;

class Surface : public ImageDecoder {
public:
	Surface(unsigned int width, unsigned int height, TextureFormat format);
	Surface(unsigned int width, unsigned int height, unsigned int depth, TextureFormat format);

	[[deprecated]] void *getData(unsigned int layer = 0);

	/*!
	 * Get the image data as span of arbitrary type
	 * @tparam T The type of the elements of the span to create
	 * @param layer The layer, which to retrieve
	 * @return A span containing the data of the image in the given type
	 */
	template<typename T>
	std::span<T> getData(unsigned int layer = 0) {
		assert(_layers[layer].data[0].size() % sizeof(T) == 0);

		return Common::toSpan<T>(_layers[layer].data[0]);
	};
};

}

#endif //AWE_SURFACE_H
