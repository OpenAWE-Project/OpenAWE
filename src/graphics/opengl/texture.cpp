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

#include <assert.h>
#include <stdexcept>
#include <iostream>
#include <fmt/format.h>

#include "src/common/exception.h"

#include "src/graphics/opengl/texture.h"

namespace Graphics::OpenGL {

Texture::Texture(GLenum type) : _type(type) {
	glCreateTextures(_type, 1, &_id);
}

Texture::Texture(const ImageDecoder &decoder, GLuint id) : _id(id), _freeTexture(false) {
	load(decoder);
}

Texture::Texture(unsigned int width, unsigned int height) : _type(GL_TEXTURE_2D), _freeTexture(true) {
	glCreateTextures(GL_TEXTURE_2D, 1, &_id);

	bind();

	glTexParameteri(_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(_type, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(_type, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA16F,
			width,
			height,
			0,
			GL_RGBA,
			GL_FLOAT,
			nullptr
	);
}

Texture::~Texture() {
    if (_freeTexture)
	    glDeleteTextures(1, &_id);
}

void Texture::allocate(TextureFormat textureFormat, unsigned int width, unsigned int height) {
	if (_type != GL_TEXTURE_2D)
		throw CreateException("Can only allocate 2d textures");

	bind();

	glTexParameteri(_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(_type, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(_type, GL_TEXTURE_WRAP_T, GL_REPEAT);

	GLenum format, internalFormat = 0, type = 0;
	getParameters(textureFormat, format, internalFormat, type);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		internalFormat,
		width,
		height,
		0,
		format,
		type,
		nullptr
	);
}

void Texture::load(unsigned int xoffset, unsigned int yoffset, const ImageDecoder &decoder) {
	bool layered = decoder.getNumMipMaps() > 1;

	switch (decoder.getType()) {
		case kTextureCube:
			_type = GL_TEXTURE_CUBE_MAP;
			break;
		case kTexture2D:
			if (layered)
				_type = GL_TEXTURE_2D_ARRAY;
			else
				_type = GL_TEXTURE_2D;
			break;
		case kTexture3D:
			_type = GL_TEXTURE_3D;
			break;

		default:
			throw Common::Exception("Invalid image type", decoder.getType());
	}

	bind();

	glTexParameteri(_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(_type, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(_type, GL_TEXTURE_WRAP_T, GL_REPEAT);

	GLenum format, internalFormat = 0, type = 0;
	getParameters(decoder.getFormat(), format, internalFormat, type);

	for (unsigned int i = 0; i < decoder.getNumMipMaps(); ++i) {
		const auto &mipmap = decoder.getMipMap(i);
		assert(mipmap.width != 0 && mipmap.height != 0);

		if (decoder.isCompressed()) {
			glCompressedTexSubImage2D(
				_type,
				i,
				xoffset,
				yoffset,
				mipmap.width,
				mipmap.height,
				format,
				mipmap.dataSize,
				mipmap.data[0]
			);
		} else {
			if (layered) {
				glTexSubImage3D(
					_type,
					i,
					xoffset,
					yoffset,
					0,
					mipmap.width,
					mipmap.height,
					i,
					format,
					type,
					mipmap.data[0]
				);
			} else {
				glTexSubImage2D(
					_type,
					i,
					xoffset,
					yoffset,
					mipmap.width,
					mipmap.height,
					format,
					type,
					mipmap.data[0]
				);
			}
		}
	}

	assert(glGetError() == GL_NO_ERROR);
}

void Texture::load(const ImageDecoder &decoder) {
	bool layered = decoder.getMipMap().data.size() > 1;

	switch (decoder.getType()) {
		case kTextureCube:
			_type = GL_TEXTURE_CUBE_MAP;
			break;
		case kTexture2D:
			if (layered)
				_type = GL_TEXTURE_2D_ARRAY;
			else
				_type = GL_TEXTURE_2D;
			break;
		case kTexture3D:
			_type = GL_TEXTURE_3D;
			break;

		default:
			throw Common::Exception("Invalid image type", decoder.getType());
	}

	bind();

	glTexParameteri(_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(_type, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(_type, GL_TEXTURE_WRAP_T, GL_REPEAT);

	GLenum format, internalFormat = 0, type = 0;
	getParameters(decoder.getFormat(), format, internalFormat, type);

	GLuint level = 0;
	for (unsigned int i = 0; i < decoder.getNumMipMaps(); ++i) {
		const auto &mipmap = decoder.getMipMap(i);
			assert(mipmap.width != 0 && mipmap.height != 0);

			if (decoder.getType() == kTextureCube) {
				if (decoder.isCompressed()) {
					glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, level, format, mipmap.width, mipmap.height, 0,
										   mipmap.dataSize, mipmap.data[0]);
					glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, level, format, mipmap.width, mipmap.height, 0,
										   mipmap.dataSize, mipmap.data[1]);
					glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, level, format, mipmap.width, mipmap.height, 0,
										   mipmap.dataSize, mipmap.data[2]);
					glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, level, format, mipmap.width, mipmap.height, 0,
										   mipmap.dataSize, mipmap.data[3]);
					glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, level, format, mipmap.width, mipmap.height, 0,
										   mipmap.dataSize, mipmap.data[4]);
					glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, level, format, mipmap.width, mipmap.height, 0,
										   mipmap.dataSize, mipmap.data[5]);
				} else {
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, level, internalFormat, mipmap.width, mipmap.height, 0,
								 format, type, mipmap.data[0]);
					glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, level, internalFormat, mipmap.width, mipmap.height, 0,
								 format, type, mipmap.data[1]);
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, level, internalFormat, mipmap.width, mipmap.height, 0,
								 format, type, mipmap.data[2]);
					glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, level, internalFormat, mipmap.width, mipmap.height, 0,
								 format, type, mipmap.data[3]);
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, level, internalFormat, mipmap.width, mipmap.height, 0,
								 format, type, mipmap.data[4]);
					glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, level, internalFormat, mipmap.width, mipmap.height, 0,
								 format, type, mipmap.data[5]);
				}
			} else {
				if (decoder.isCompressed()) {
					glCompressedTexImage2D(
							_type,
							level,
							format,
							mipmap.width,
							mipmap.height,
							0,
							mipmap.dataSize,
							mipmap.data[0]
					);
				} else {
					if (layered) {
						glTexSubImage3D(
								_type,
								level,
								0,
								0,
								0,
								mipmap.width,
								mipmap.height,
								i,
								format,
								type,
								mipmap.data[0]
						);
					} else {
						glTexImage2D(
								_type,
								level,
								internalFormat,
								mipmap.width,
								mipmap.height,
								0,
								format,
								type,
								mipmap.data[0]
						);
					}
				}
			}

			level += 1;
	}

	assert(glGetError() == GL_NO_ERROR);
}

void Texture::bind() {
	//assert(glIsTexture(_id) == GL_TRUE);
	glBindTexture(_type, _id);
}

void Texture::getParameters(
	TextureFormat textureFormat,
	GLenum &format,
	GLenum &internalFormat,
	GLenum &type
) const {
	switch (textureFormat) {
		case kR8:
			internalFormat = GL_R8;
			format = GL_RED;
			type = GL_UNSIGNED_BYTE;
			break;
		case kRG16:
			internalFormat = GL_RG16;
			format = GL_RG;
			type = GL_UNSIGNED_SHORT;
			break;
		case kRG8:
			internalFormat = GL_RG8;
			format = GL_RG;
			type = GL_UNSIGNED_BYTE;
			break;
		case kRGB8:
			internalFormat = GL_RGB;
			format = GL_RGB;
			type = GL_UNSIGNED_BYTE;
			break;
		case kA1RGB5:
			internalFormat = GL_RGB5_A1;
			format = GL_BGRA;
			type = GL_UNSIGNED_SHORT_1_5_5_5_REV;
			break;
		case kRGBA8:
			internalFormat = GL_RGBA;
			format = GL_BGRA;
			type = GL_UNSIGNED_BYTE;
			break;

		case kBC1:
			format = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
			break;
		case kBC2:
			format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			break;
		case kBC3:
			format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			break;

		default:
			throw std::runtime_error("Invalid texture format");
	}
}

}
