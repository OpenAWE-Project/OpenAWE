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

#ifndef AWE_TERRAINDATAFILE_H
#define AWE_TERRAINDATAFILE_H

#include <vector>

#include <glm/glm.hpp>

#include "src/common/readstream.h"

namespace AWE {

class TerrainDataFile {
public:
	struct Tileset {
		uint16_t colorTile1, colorTile2, colorTile3;
		uint16_t normalTile1, normalTile2, normalTile3;
	};

	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		float displacementFactor;
	};

	struct Polygon {
		Common::BoundSphere boundSphere;
		std::vector<uint32_t> indices;
		std::vector<glm::vec2> texCoords[4];
		std::vector<glm::vec3> tangents[3];
		uint16_t tilesetId;
		uint16_t flags;
	};

	struct Blend {
		uint32_t polygonId, size;
		std::vector<uint16_t> data;
	};

	TerrainDataFile(Common::ReadStream &readStream);

	const std::vector<Vertex> &getVertices();

private:
	std::vector<Vertex> _vertices;
	std::vector<Polygon> _polygons;
	std::vector<std::string> _textures;

	std::vector<Blend> _colorBlends;
	std::vector<Blend> _displacementMaps;
};

}

#endif //AWE_TERRAINDATAFILE_H
