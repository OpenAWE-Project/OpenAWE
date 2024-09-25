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
#include "src/graphics/opengl/task.h"
#include "src/graphics/opengl/gltexture.h"

namespace Graphics::OpenGL {

class Texture : public Graphics::Texture, public GLTexture {
public:
	Texture(TaskQueue &tasks, GLenum type, const std::string &label);
	Texture(TaskQueue &tasks, unsigned int width, unsigned int height, TextureFormat textureFormat, const std::string &label);
	~Texture();

	void bind() override;
	void bindImage(unsigned int unit = 0);

	void allocate(TextureFormat textureFormat, unsigned int width, unsigned int height) override;

	void load(unsigned int xoffset, unsigned int yoffset, ImageDecoder &&decoder) override;

	void load(ImageDecoder &&decoder) override;

	void setWrapMode(WrapMode s, WrapMode t, WrapMode u) override;

private:
	friend class Framebuffer;

	GLuint _id;
	GLenum _type;

	TaskQueue  &_tasks;
};

}

#endif //AWE_TEXTURE_H
