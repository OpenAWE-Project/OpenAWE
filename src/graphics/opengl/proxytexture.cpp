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

#include "src/graphics/opengl/proxytexture.h"

namespace Graphics::OpenGL {

void ProxyTexture::bind() {
	GLTexturePtr glTexture = std::dynamic_pointer_cast<Graphics::OpenGL::GLTexture>(_texture);
	if (!glTexture)
		throw CreateException("Texture for proxy texture is not an opengl compatible texture");
	glTexture->bind();
}

void ProxyTexture::allocate(TextureFormat textureFormat, unsigned int width, unsigned int height) {
	_texture->allocate(textureFormat, width, height);
}

void ProxyTexture::load(unsigned int xoffset, unsigned int yoffset, ImageDecoder &&decoder) {
	_texture->load(xoffset, yoffset, std::move(decoder));
}

void ProxyTexture::load(ImageDecoder &&decoder) {
	_texture->load(std::move(decoder));
}

void ProxyTexture::assign(TexturePtr texture) {
	_texture = texture;
}

void ProxyTexture::setWrapMode(WrapMode s, WrapMode t, WrapMode r) {
	_texture->setWrapMode(s, t, r);
}

} // End of namespace Graphics::OpenGL