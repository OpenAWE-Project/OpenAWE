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

#ifndef AWE_LOCALECONFIG_H
#define AWE_LOCALECONFIG_H

#include <string>
#include <map>

#include "src/common/platform.h"
#include "src/common/readstream.h"
#include "src/common/xml.h"

/*!
 * \brief Class for reading locale config files
 *
 * This class reads the locale config files from all games in the xml format and offers the configuration Data of every
 * language
 */
class LocaleConfig {
public:
	struct Config {
		std::string folder;
		unsigned int voiceoverChannel;
		bool subtitlesDefault;
		bool useOnlyOneFont;
	};

	LocaleConfig(Common::ReadStream &locale);

	/*!
	 * Get the entry data for a specific language. If the language is not available, this method returns the english
	 * configuration as default
	 *
	 * \param l The language to get the config for
	 * \return The config data for the requested language
	 */
	const Config &getLanguageConfig(Common::Language l) const;

private:
	std::map<Common::Language, Config> _entries;
};


#endif //AWE_LOCALECONFIG_H
