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

/*!
 * \brief Class for parsing cid_terraindata.bin files
 *
 * This class parses a cid_terraindata file and offers the information to use them inside the graphics system. It only
 * loads version 25 of the files. There are two major variants of the file format, the low detail and the high detail
 * files, denoted by a bool at the beginning of the file. The low detail files contain the fundamental data of the
 * terrain, including tileset information, vertices. basic polygon information and bsp tree information. Both, low
 * detail and high detail files contain low and high detail blending and geonormal maps.
 *
 * The terrain is organized in polygons which are mostly quads and in some cases triangles from which the terrain is
 * build up. Every one of these polygons has a blending map, which helps blending a maximum of three textures together.
 */
class TerrainDataFile {
public:
	/*!
	 * A tileset, containing indices to three color and normal textures used in specific polygons
	 */
	struct Tileset {
		uint16_t colorTiles[3];
		uint16_t normalTiles[3];
	};

	/*!
	 * A vertex of a polygon consisting of a position, a normal and a displacement factor
	 */
	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		float displacementFactor;
	};

	/*!
	 * A simple struct containing either a blending map or a geonormal map
	 */
	struct Blend {
		uint32_t id;
		uint32_t size;
		std::vector<uint16_t> data;
	};

	/*!
	 * Class containing the basic building block of a terrain
	 */
	struct Polygon {
		Common::BoundSphere boundSphere;
		std::vector<uint32_t> indices;
		std::vector<glm::vec2> texCoords[4];
		std::vector<glm::vec3> tangents[3];
		uint16_t tilesetId;
		uint16_t flags;
	};

	/*!
	 * Parse a terrain data file from a given readstream
	 * \param readStream The readstream from which to parse terrain data
	 */
	TerrainDataFile(Common::ReadStream &readStream);

	/*!
	 * If this file is a high detail terrain data file
	 * \return If this loaded terrain data is high detail
	 */
	bool isHighDetail() const;

	/*!
	 * Get a vector of all tilesets found in this file
	 * \return A vector of all tilesets from this file
	 */
	const std::vector<Tileset> &getTilesets() const;

	/*!
	 * Get the vector of all vertices in this file
	 * \return A vector of all vertices in this file
	 */
	const std::vector<Vertex> &getVertices() const;

	/*!
	 * Get the vector of all polygons in this file
	 * \return A vector of all polygons in this file
	 */
	const std::vector<Polygon> &getPolygons() const;

	/*!
	 * Get the vector of all texture file names used by this terrain data
	 * \return A vector of all texture file names
	 */
	const std::vector<std::string> &getTextures() const;

	/*!
	 * Get a vector of all geo normal maps contained in this file
	 * \return A vector of all geo normal maps in this file
	 */
	const std::vector<Blend> &getGeonormalMaps() const;

	/*!
	 * Get a vector of all blending maps contained in this file
	 * \return A vector of all blending maps in this file
	 */
	const std::vector<Blend> &getBlendMaps() const;

private:
	bool _hd;

	std::vector<Tileset> _tilesets;
	std::vector<Vertex> _vertices;
	std::vector<Polygon> _polygons;
	std::vector<std::string> _textures;

	std::vector<Blend> _geonormalMaps;
	std::vector<Blend> _blendMaps;
};

}

#endif //AWE_TERRAINDATAFILE_H
