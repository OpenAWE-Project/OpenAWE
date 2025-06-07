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

#include "src/common/exception.h"

#include "src/graphics/opengl/task.h"
#include "src/graphics/opengl/texture.h"

namespace Graphics::OpenGL {

static void getParameters(
		TextureFormat textureFormat,
		GLenum &format,
		GLenum &internalFormat,
		GLenum &type
) {
	switch (textureFormat) {
		case kR8:
			internalFormat = GL_R8;
			format = GL_RED;
			type = GL_UNSIGNED_BYTE;
			break;
		case kR32F:
			internalFormat = GL_R32F;
			format = GL_RED;
			type = GL_FLOAT;
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
			internalFormat = GL_RGB8;
			format = GL_RGB;
			type = GL_UNSIGNED_BYTE;
			break;
		case kA1RGB5:
			internalFormat = GL_RGB5_A1;
			format = GL_BGRA;
			type = GL_UNSIGNED_SHORT_1_5_5_5_REV;
			break;
		case kRGBA8:
			internalFormat = GL_RGBA8;
			format = GL_BGRA;
			type = GL_UNSIGNED_BYTE;
			break;

		case kRGBA16F:
			internalFormat = GL_RGBA16F;
			format = GL_RGBA;
			type = GL_HALF_FLOAT;
			break;
		case kRGBA32F:
			internalFormat = GL_RGBA32F;
			format = GL_RGBA;
			type = GL_FLOAT;
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
			throw Common::SourceException("Invalid texture format {}", Common::toUnderlying(textureFormat));
	}
}

static GLint getTextureWrapMode(WrapMode mode) {
	switch (mode) {
		default: [[fallthrough]];
		case WrapMode::kRepeat:
			return GL_REPEAT;

		case WrapMode::kClamp:
			return GL_CLAMP_TO_EDGE;
	}
}


Texture::Texture(TaskQueue &tasks, GLenum type, const std::string &label) : _id(std::make_shared<GLuint>(0)), _type(type), _tasks(tasks) {
	//tasks.emplace_back(std::make_unique<TextureCreationTask>(_id, _type, label));
	tasks.emplace_back([=]() {
		glGenTextures(1, _id.get());
		glBindTexture(_type, *_id);

		if (GLAD_GL_KHR_debug && !label.empty())
			glObjectLabel(GL_TEXTURE, *_id, static_cast<GLsizei>(label.size()), label.c_str());
	});
}

Texture::Texture(TaskQueue &tasks, unsigned int width, unsigned int height, TextureFormat textureFormat,
				 const std::string &label) : _id(std::make_shared<GLuint>(0)), _type(GL_TEXTURE_2D), _tasks(tasks) {
	glGenTextures(1, _id.get());

	bind();

	glTexParameteri(_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(_type, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(_type, GL_TEXTURE_WRAP_T, GL_REPEAT);

	GLenum format, internalFormat = 0, type = 0;
	getParameters(textureFormat, format, internalFormat, type);

	glTexImage2D(
			_type,
			0,
			internalFormat,
			width,
			height,
			0,
			format,
			type,
			nullptr
	);

	if (GLAD_GL_KHR_debug && !label.empty())
		glObjectLabel(GL_TEXTURE, *_id, static_cast<GLsizei>(label.size()), label.c_str());
}

Texture::~Texture() {
	_tasks.emplace_back([=]() {
		glDeleteTextures(1, _id.get());
	});
}

void Texture::allocate(TextureFormat textureFormat, unsigned int width, unsigned int height) {
	if (_type != GL_TEXTURE_2D)
		throw CreateException("Can only allocate 2d textures");

	GLenum format, internalFormat = 0, type = 0;
	getParameters(textureFormat, format, internalFormat, type);

	_tasks.emplace_back([=, target = _type, id = _id](){
		glBindTexture(target, *id);

		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);

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
	});
}

void Texture::load(unsigned int xoffset, unsigned int yoffset, ImageDecoder &&decoder) {
	GLenum format, internalFormat = 0, type = 0;
	getParameters(decoder.getFormat(), format, internalFormat, type);

	_tasks.emplace_back([=, target = _type, id = _id]() {
		glBindTexture(target, *id);

		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		if (decoder.getNumMipMaps() > 1) {
			glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, decoder.getNumMipMaps() - 1);
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		} else {
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}

		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);

		GLenum format, internalFormat = 0, type = 0;
		getParameters(decoder.getFormat(), format, internalFormat, type);

		for (unsigned int i = 0; i < decoder.getNumMipMaps(); ++i) {
			const auto &mipmap = decoder.getMipMap(i);
			assert(mipmap.width != 0 && mipmap.height != 0);

			const auto &imageData = mipmap.data[0];

			if (decoder.isCompressed()) {
				glCompressedTexSubImage2D(
						_type,
						i,
						xoffset,
						yoffset,
						mipmap.width,
						mipmap.height,
						format,
						imageData.size(),
						imageData.data()
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
						imageData.data()
				);
			}
		}
	});
}

void Texture::load(ImageDecoder &&decoder) {
	switch (decoder.getType()) {
		case kTexture2D:
			_type = GL_TEXTURE_2D;
			break;
		case kTexture3D:
			_type = GL_TEXTURE_3D;
			break;
		case kTextureCube:
			_type = GL_TEXTURE_CUBE_MAP;
			break;

		default:
			throw Common::Exception("Invalid image type {}", Common::toUnderlying(decoder.getType()));
	}

	GLenum format, internalFormat = 0, type = 0;
	getParameters(decoder.getFormat(), format, internalFormat, type);

	_tasks.emplace_back([=, target = _type, id = _id] {
		glBindTexture(target, *id);

		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		if (decoder.getNumMipMaps() > 1) {
			glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, decoder.getNumMipMaps() - 1);
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		} else {
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}

		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);

		if (decoder.getType() == kTexture3D)
			glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_REPEAT);

		GLuint level = 0;
		for (unsigned int i = 0; i < decoder.getNumMipMaps(); ++i) {
			const auto &mipmap = decoder.getMipMap(i);
			assert(mipmap.width != 0 && mipmap.height != 0);

			const auto &imageData = mipmap.data[0];

			switch (decoder.getType()) {
				case kTexture2D:
					if (decoder.isCompressed()) {
						glCompressedTexImage2D(
								target,
								level,
								format,
								mipmap.width,
								mipmap.height,
								0,
								imageData.size(),
								imageData.data()
						);
					} else {
						glTexImage2D(
								target,
								level,
								internalFormat,
								mipmap.width,
								mipmap.height,
								0,
								format,
								type,
								imageData.data()
						);
					}
					break;

				case kTexture3D:
					if (decoder.isCompressed()) {
						glCompressedTexImage3D(
								target,
								level,
								format,
								mipmap.width,
								mipmap.height,
								mipmap.depth,
								0,
								imageData.size(),
								imageData.data()
						);
					} else {
						glTexImage3D(
								target,
								level,
								internalFormat,
								mipmap.width,
								mipmap.height,
								mipmap.depth,
								0,
								format,
								type,
								imageData.data()
						);
					}
					break;

				case kTextureCube:
					if (decoder.isCompressed()) {
						glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, level, format, mipmap.width,
											   mipmap.height, 0,
											   mipmap.data[0].size(), mipmap.data[0].data());
						glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, level, format, mipmap.width,
											   mipmap.height, 0,
											   mipmap.data[1].size(), mipmap.data[1].data());
						glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, level, format, mipmap.width,
											   mipmap.height, 0,
											   mipmap.data[2].size(), mipmap.data[2].data());
						glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, level, format, mipmap.width,
											   mipmap.height, 0,
											   mipmap.data[3].size(), mipmap.data[3].data());
						glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, level, format, mipmap.width,
											   mipmap.height, 0,
											   mipmap.data[4].size(), mipmap.data[4].data());
						glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, level, format, mipmap.width,
											   mipmap.height, 0,
											   mipmap.data[5].size(), mipmap.data[5].data());
					} else {
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, level, internalFormat, mipmap.width, mipmap.height,
									 0,
									 format, type, mipmap.data[0].data());
						glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, level, internalFormat, mipmap.width, mipmap.height,
									 0,
									 format, type, mipmap.data[1].data());
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, level, internalFormat, mipmap.width, mipmap.height,
									 0,
									 format, type, mipmap.data[2].data());
						glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, level, internalFormat, mipmap.width, mipmap.height,
									 0,
									 format, type, mipmap.data[3].data());
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, level, internalFormat, mipmap.width, mipmap.height,
									 0,
									 format, type, mipmap.data[4].data());
						glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, level, internalFormat, mipmap.width, mipmap.height,
									 0,
									 format, type, mipmap.data[5].data());
					}
					break;
			}

			level += 1;
		}
	});
}

void Texture::setWrapMode(WrapMode s, WrapMode t, WrapMode r) {
	_tasks.emplace_back([=, type = _type, id = _id](){
		glBindTexture(type, *id);

		glTexParameteri(type, GL_TEXTURE_WRAP_S, getTextureWrapMode(s));
		glTexParameteri(type, GL_TEXTURE_WRAP_T, getTextureWrapMode(t));

		if (type == GL_TEXTURE_3D)
			glTexParameteri(type, GL_TEXTURE_WRAP_R, getTextureWrapMode(r));
	});
}

void Texture::bind() {
	glBindTexture(_type, *_id);
}

void Texture::bindImage(unsigned int unit, GLenum rw, GLenum format) {
	glBindImageTexture(
			unit,
			*_id,
			0,
			GL_FALSE,
			0,
			rw,
			format
	);
}

}
