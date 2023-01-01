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

#include "src/localeconfig.h"

typedef std::tuple<unsigned int, unsigned int> Locale;

static constexpr Locale kXCEnglish         = Locale(1, 0);
static constexpr Locale kXCJapanese        = Locale(2, 0);
static constexpr Locale kXCGerman          = Locale(3, 0);
static constexpr Locale kXCFrench          = Locale(4, 0);
static constexpr Locale kXCSpanish         = Locale(5, 0);
static constexpr Locale kXCSpanishChile    = Locale(5, 6);
static constexpr Locale kXCSpanishColombia = Locale(5, 8);
static constexpr Locale kXCSpanishMexico   = Locale(5, 22);
static constexpr Locale kXCItalian         = Locale(6, 0);
static constexpr Locale kXCKorean          = Locale(7, 0);
static constexpr Locale kXCTChinese        = Locale(8, 0);
static constexpr Locale kXCPolish          = Locale(11, 0);
static constexpr Locale kXCRussian         = Locale(12, 0);
static constexpr Locale kXCCzech           = Locale(1, 9);
static constexpr Locale kXCHungarian       = Locale(1, 16);

LocaleConfig::LocaleConfig(Common::ReadStream &locale) {
	Common::XML lc;

	lc.read(locale);

	Common::XML::Node root = lc.getRootNode();
	if (root.name != "locale_config")
		throw std::runtime_error("Invalid locale config");

	for (auto &config : root.children) {
		if (config.name != "config")
			throw std::runtime_error("Invalid locale config tag");

		const auto langId = config.getInt("xgetlanguage_id");
		const auto localeId = config.getInt("xgetlocale_id");
		const auto locale = Locale(langId, localeId);

		Config entry;
		entry.folder = config.properties["folder"];
		entry.voiceoverChannel = config.getInt("bink_voiceover_channel");
		entry.subtitlesDefault = config.getInt("subtitles_default") != 0;
		entry.useOnlyOneFont = config.getInt("use_only_one_font") != 0;

		Common::Language lang;
		if (locale == kXCEnglish)
			lang = Common::kEnglish;
		else if (locale == kXCJapanese)
			lang = Common::kJapanese;
		else if (locale == kXCGerman)
			lang = Common::kGerman;
		else if (locale == kXCFrench)
			lang = Common::kFrench;
		else if (locale == kXCSpanish || locale == kXCSpanishChile || locale == kXCSpanishColombia || locale == kXCSpanishMexico)
			lang = Common::kSpanish;
		else if (locale == kXCItalian)
			lang = Common::kItalian;
		else if (locale == kXCKorean)
			lang = Common::kKorean;
		else if (locale == kXCTChinese)
			lang = Common::kChinese;
		else if (locale == kXCPolish)
			lang = Common::kPolish;
		else if (locale == kXCRussian)
			lang = Common::kRussian;
		else if (locale == kXCCzech)
			lang = Common::kCzech;
		else if (locale == kXCHungarian)
			lang = Common::kHungarian;
		else
			continue;

		_entries[lang] = entry;
	}
}

const LocaleConfig::Config &LocaleConfig::getLanguageConfig(Common::Language l) const {
	const auto iter = _entries.find(l);

	// If the language is not supported in this game, always use the english one
	if (iter == _entries.end())
		return _entries.find(Common::kEnglish)->second;

	return iter->second;
}
