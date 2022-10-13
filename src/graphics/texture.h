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

#ifndef OPENAWE_TEXTURE_H
#define OPENAWE_TEXTURE_H

#include <memory>

#include "src/common/types.h"

#include "src/graphics/images/decoder.h"

namespace Graphics {

class Texture;
class ProxyTexture;

typedef std::shared_ptr<Texture> TexturePtr;
typedef std::shared_ptr<ProxyTexture> ProxyTexturePtr;

class Texture : Common::Noncopyable {
public:
	/*!
	 * Allocate memory for the texture using a format and a size. This removes all previously allocated memory and the
	 * data contained in it.
	 * \param textureFormat The texture format of the data to format the texture for
	 * \param width The width of the new texture to format
	 * \param height The height of the new texture to format
	 */
	virtual void allocate(TextureFormat textureFormat, unsigned int width, unsigned int height) = 0;

	/*!
	 * Load an image into the texture using a certain offset
	 * \param xoffset The offset in x direction
	 * \param yoffset The offset in y direction
	 * \param decoder The image to copy into the texture
	 */
	virtual void load(unsigned int xoffset, unsigned int yoffset, const Graphics::ImageDecoder &decoder) = 0;

	/*!
	 * Load an image into the texture and reallocate the texture in regards to the images size.
	 * \param decoder The image to load into the texture
	 */
	virtual void load(const Graphics::ImageDecoder &decoder) = 0;
};

/*!
 * \brief Texture for proxying another texture
 *
 * This class proxies another texture through. This is intended for video ülayback to allow another class modifying the
 * texture in a fast way.
 */
class ProxyTexture : public Texture {
public:
	/*!
	 * Asssign a texture to this proxy texture
	 * @param texture The texture
	 */
	virtual void assign(TexturePtr texture) = 0;
};

}

#endif //OPENAWE_TEXTURE_H
