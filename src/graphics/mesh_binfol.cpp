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

#include <stdexcept>

#include <fmt/format.h>

#include "mesh_binfol.h"

namespace Graphics {

BINFOLMesh::BINFOLMesh(Common::ReadStream *binfol) {
	uint32_t version = binfol->readUint32LE();
	if (version != 19)
		throw std::runtime_error(fmt::format("Unsupported version {}. Expected 19", version));

	uint32_t meshDataSize = binfol->readUint32LE();
	BINMSHMesh::load(binfol->readStream(meshDataSize));

	uint32_t billboard1Length = binfol->readUint32LE();
	std::string billboard1 = binfol->readFixedSizeString(billboard1Length, true);
	uint32_t billboard2Length = binfol->readUint32LE();
	std::string billboard2 = binfol->readFixedSizeString(billboard2Length, true);
	uint32_t billboard3Length = binfol->readUint32LE();
	std::string billboard3 = binfol->readFixedSizeString(billboard3Length, true);

	// TODO: Here are way more values
}

}
