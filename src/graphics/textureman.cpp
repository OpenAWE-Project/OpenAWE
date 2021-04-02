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

#include <memory>
#include <regex>

#include "src/graphics/images/tex.h"
#include "src/graphics/images/dds.h"
#include "src/graphics/textureman.h"

#include "src/awe/resman.h"

#include "src/graphics/gfxman.h"

namespace Graphics {

Common::UUID TextureManager::getTexture(const std::string &path) {
	if (!std::regex_match(path, std::regex(".*(\\.tex|\\.tex_lo)$")))
		return Common::UUID::generateNil();

	if (_textures.find(path) != _textures.end())
		return _textures[path];

	std::unique_ptr<Common::ReadStream> stream(ResMan.getResource(path));

	std::unique_ptr<ImageDecoder> decoder;
	// In Quantum break (and probably above) .tex files are actually dds files
	uint32_t ddsIdentifier = stream->readUint32LE();
	stream->seek(0);
	if (ddsIdentifier == MKTAG('D', 'D', 'S', 0x20))
		decoder = std::make_unique<DDS>(stream.get());
	else
		decoder = std::make_unique<TEX>(*stream);

	return GfxMan.registerTexture(*decoder);
}

}
