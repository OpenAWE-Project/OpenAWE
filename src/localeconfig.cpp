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

#include "src/common/xml.h"

#include "localeconfig.h"

LocaleConfig::LocaleConfig(Common::ReadStream *locale) {
	Common::XML lc;

	lc.read(*locale);

	Common::XML::Node root = lc.getRootNode();
	if (root.name != "locale_config")
		throw std::runtime_error("Invalid locale config");

	for (auto &config : root.children) {
		if (config.name != "config")
			throw std::runtime_error("Invalid locale config tag");

		Entry entry;
		entry.languageId = std::atoi(config.properties["xgetlanguage_id"].c_str());
		entry.folder = config.properties["folder"];
		entry.voiceoverChannel = std::atoi(config.properties["bink_voiceover_channel"].c_str());
	}
}
