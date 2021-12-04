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

#include <memory>
#include <cstring>

#include "surface.h"

namespace Graphics {

Surface::Surface(unsigned int width, unsigned int height, TextureFormat format) {
	_format = format;

	_layers.resize(1);
	auto &mipmaps = _layers[0];
	mipmaps.resize(1);

	Mipmap &mipmap = mipmaps[0];

	mipmap.width = width;
	mipmap.height = height;
	mipmap.dataSize = getImageSize(width, height);
	mipmap.data.resize(1);
	mipmap.data[0] = new byte[mipmap.dataSize];

	std::memset(mipmap.data[0], 0, mipmap.dataSize);
}

Surface::Surface(unsigned int width, unsigned int height, unsigned int numLayers, TextureFormat format) {
	_format = format;

	_layers.resize(numLayers);
	for (auto & layer : _layers) {
		layer.resize(1);
		auto &mipmap = layer[0];

		mipmap.width = width;
		mipmap.height = height;
		mipmap.dataSize = getImageSize(width, height);
		mipmap.data[0] = new byte [mipmap.dataSize];

		std::memset(mipmap.data[0], 0, mipmap.dataSize);
	}
}

void *Surface::getData(unsigned int layer) {
	return _layers[layer][0].data[0];
}

}
