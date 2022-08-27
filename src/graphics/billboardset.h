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

#ifndef OPENAWE_BILLBOARDSET_H
#define OPENAWE_BILLBOARDSET_H

#include "src/graphics/attributeobject.h"
#include "src/graphics/buffer.h"
#include "src/graphics/texture.h"
#include "src/graphics/model.h"

namespace Graphics {

class BillboardSet;

typedef std::shared_ptr<BillboardSet> BillboardSetPtr;

class BillboardSet : public Model {
public:
	struct BillboardType {
		glm::vec2 atlasSize, atlasPosition;
		glm::vec2 billboardSize;
	};

	struct BillboardInstance {
		glm::vec3 position;
		uint8_t type;
	};

	BillboardSet(
		const std::string &colorAtlasTexture,
		const std::vector<BillboardType> &types,
		const std::vector<BillboardInstance> &instance
	);

private:
	TexturePtr _colorAtlas;
	BufferPtr _billboardTypes;
};

} // End of namespace Graphics

#endif //OPENAWE_BILLBOARDSET_H
