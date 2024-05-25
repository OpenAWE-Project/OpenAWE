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

#include <regex>

#include "src/common/exception.h"

#include "src/awe/resman.h"

#include "src/graphics/fontman.h"
#include "src/graphics/font_binfnt.h"

namespace Graphics {

void FontManager::load(const std::string &path, const std::string &id) {
	if (!std::regex_match(path, std::regex(".*\\.binfnt")))
		throw CreateException("Invalid font file");

	std::unique_ptr<Common::ReadStream> font(ResMan.getResource(path));

	if (!font)
		throw CreateException("Font file not found: {}", path);

	_fonts[id] = std::make_unique<BINFNTFont>(*font);
}

Font &FontManager::get(const std::string &id) {
	return *_fonts[id];
}

}
