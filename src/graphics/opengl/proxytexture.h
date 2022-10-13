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

#ifndef OPENAWE_PROXYTEXTURE_H
#define OPENAWE_PROXYTEXTURE_H

#include "src/graphics/opengl/texture.h"
#include "src/graphics/opengl/gltexture.h"

namespace Graphics::OpenGL {

class ProxyTexture : public Graphics::ProxyTexture, public GLTexture {
public:
	ProxyTexture() = default;

	void allocate(TextureFormat textureFormat, unsigned int width, unsigned int height) override;

	void load(unsigned int xoffset, unsigned int yoffset, const ImageDecoder &decoder) override;

	void load(const ImageDecoder &decoder) override;

	void assign(TexturePtr texture) override;

	void bind() override;

private:
	TexturePtr _texture;
};

} // End of namespace Graphics::OpenGL

#endif //OPENAWE_PROXYTEXTURE_H
