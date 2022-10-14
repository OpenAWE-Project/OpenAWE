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

#ifndef AWE_TEXTURE_H
#define AWE_TEXTURE_H

#include "src/graphics/texture.h"
#include "src/graphics/images/decoder.h"
#include "src/graphics/opengl/opengl.h"
#include "src/graphics/opengl/gltexture.h"

namespace Graphics::OpenGL {

class Texture : public Graphics::Texture, public GLTexture {
public:
	Texture(GLenum type);
	Texture(const ImageDecoder &decoder, GLuint id);
	Texture(unsigned int width, unsigned int height);
	~Texture();

	void bind() override;

	void allocate(TextureFormat textureFormat, unsigned int width, unsigned int height) override;

	void load(unsigned int xoffset, unsigned int yoffset, const ImageDecoder &decoder) override;

	void load(const ImageDecoder &decoder) override;

private:
	void getParameters(
		TextureFormat textureFormat,
		GLenum &format,
		GLenum &internalFormat,
		GLenum &type
	) const;

	friend class Framebuffer;

	bool _freeTexture;

	GLuint _id;
	GLenum _type;
};

}

#endif //AWE_TEXTURE_H
