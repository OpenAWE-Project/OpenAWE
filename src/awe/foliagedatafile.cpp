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
#include <regex>

#include <fmt/format.h>

#include "foliagedatafile.h"

namespace AWE {

FoliageDataFile::FoliageDataFile(Common::ReadStream &foliageData) {
	const uint32_t version = foliageData.readUint32LE();
	if (version != 9)
		throw std::runtime_error(fmt::format("Unsupported version, expected 9, got {}", version));

	const uint32_t numFoliages = foliageData.readUint32LE();
	_foliages.resize(numFoliages);
	for (auto &foliage : _foliages) {
		uint32_t pathLength = foliageData.readUint32LE();
		foliage = foliageData.readFixedSizeString(pathLength, true);

		// normalize foliage path
		foliage = std::regex_replace(foliage, std::regex("\\\\"), "/");
		foliage = std::regex_replace(foliage, std::regex("data/"), "");
	}

	const uint32_t numInstances = foliageData.readUint32LE();
	_instances.resize(numInstances);
	for (auto &instance : _instances) {
		instance.position.x = foliageData.readIEEEFloatLE();
		instance.position.y = foliageData.readIEEEFloatLE();
		instance.position.z = foliageData.readIEEEFloatLE();

		foliageData.skip(1); // Unknown

		instance.foliageId = foliageData.readByte();

		foliageData.skip(6); // Unknown
	}

	_aabb.xmin = foliageData.readIEEEFloatLE();
	_aabb.ymin = foliageData.readIEEEFloatLE();
	_aabb.zmin = foliageData.readIEEEFloatLE();
	_aabb.xmax = foliageData.readIEEEFloatLE();
	_aabb.ymax = foliageData.readIEEEFloatLE();
	_aabb.zmax = foliageData.readIEEEFloatLE();

	// TODO: There are 16 unknown floats appended to this file
}

const std::vector<std::string> &FoliageDataFile::getFoliages() const {
	return _foliages;
}

const std::vector<FoliageDataFile::Instance> &FoliageDataFile::getInstances() const {
	return _instances;
}

const Common::BoundBox &FoliageDataFile::getAabb() const {
	return _aabb;
}

}