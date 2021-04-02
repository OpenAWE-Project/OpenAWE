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

#ifndef AWE_FONTMANAGER_H
#define AWE_FONTMANAGER_H

#include <string>
#include <map>
#include <memory>

#include "src/common/singleton.h"

#include "src/graphics/font.h"

namespace Graphics {

class FontManager : public Common::Singleton<FontManager> {
public:
	void load(const std::string &path, const std::string &id);

	Font &get(const std::string &id);

private:
	std::map<std::string, std::unique_ptr<Font>> _fonts;
};

} // End of namespace Graphics

#define FontMan Graphics::FontManager::instance()

#endif //AWE_FONTMANAGER_H
