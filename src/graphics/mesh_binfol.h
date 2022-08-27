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

#ifndef OPENAWE_MESHBINFOL_H
#define OPENAWE_MESHBINFOL_H

#include "src/common/readstream.h"

#include "src/graphics/mesh_binmsh.h"

namespace Graphics {

class BINFOLMesh;

typedef std::shared_ptr<BINFOLMesh> BINFOLMeshPtr;

class BINFOLMesh : public BINMSHMesh {
public:
	BINFOLMesh(Common::ReadStream *binfol);

	/*!
	 * Get the color atlas texture file path for this foliage meshs billboard
	 * \return The path to the color atlas texture
	 */
	const std::string &getColorAtlas() const;

	/*!
	 * Get the size of the billboard as 2d vector
	 * \return The size of the billboard
	 */
	const glm::vec2 &getBillboardSize() const;

	/*!
	 * Get the size of the part of the billboard texture in the atlas
	 * \return The textures size in the atlas texture
	 */
	const glm::vec2 &getAtlasSize() const;

	/*!
	 * Get the position of the billboard texture in the atlas
	 * \return The position of the billboard texture in the atlas texture
	 */
	const glm::vec2 &getAtlasPosition() const;

private:
	std::string _colorAtlas;

	glm::vec2 _billboardSize;
	glm::vec2 _atlasSize, _atlasPosition;
};

}

#endif //OPENAWE_MESHBINFOL_H
