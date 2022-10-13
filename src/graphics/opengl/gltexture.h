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

#ifndef OPENAWE_GLTEXTURE_H
#define OPENAWE_GLTEXTURE_H

#include "src/graphics/texture.h"

namespace Graphics::OpenGL {

class GLTexture;

typedef std::shared_ptr<GLTexture> GLTexturePtr;

/*!
 * \brief base class for all possible opengl textures
 *
 * This class represents the interface textures need to have for usage in opengl which basically boils down to the bind
 * method
 */
class GLTexture : public Common::Noncopyable {
public:
	/*!
	 * Bind this texture
	 */
	virtual void bind() = 0;
};

} // End of namespace Graphics::OpenGL

#endif //OPENAWE_GLTEXTURE_H
