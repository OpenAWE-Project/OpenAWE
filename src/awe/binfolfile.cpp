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

#include "src/awe/path.h"
#include "src/awe/binfolfile.h"

namespace AWE {

BINFOLFile::BINFOLFile(Common::ReadStream &binfol) {

	const auto version = binfol.readUint32LE();
	if (version != 19)
		throw std::runtime_error(fmt::format("Unsupported version {}. Expected 19", version));

	const auto meshDataSize = binfol.readUint32LE();
	std::unique_ptr<Common::ReadStream> binmshStream(binfol.readStream(meshDataSize));
	load(*binmshStream);

	const auto colorAtlasNameLength = binfol.readUint32LE();
	_colorAtlasFile = binfol.readFixedSizeString(colorAtlasNameLength, true);
	const auto billboard2Length = binfol.readUint32LE();
	const auto billboard2 = binfol.readFixedSizeString(billboard2Length, true);
	const auto billboard3Length = binfol.readUint32LE();
	const auto billboard3 = binfol.readFixedSizeString(billboard3Length, true);

	_colorAtlasFile = AWE::getNormalizedPath(_colorAtlasFile);

	_billboardSize = binfol.read<glm::vec2>();
	_atlasPosition = binfol.read<glm::vec2>();
	_atlasSize     = binfol.read<glm::vec2>();

	// TODO: Here are way more values
}

const std::string &BINFOLFile::getColorAtlasFile() const {
	return _colorAtlasFile;
}

const glm::vec2 &BINFOLFile::getBillboardSize() const {
	return _billboardSize;
}

const glm::vec2 &BINFOLFile::getAtlasPosition() const {
	return _atlasPosition;
}

const glm::vec2 &BINFOLFile::getAtlasSize() const {
	return _atlasSize;
}

} // End of namespace AWE