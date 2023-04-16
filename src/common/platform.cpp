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

#include <locale>
#include <regex>

#include <GLFW/glfw3.h>

#include "src/common/platform.h"
#include "src/common/exception.h"
#include "src/common/types.h"
#include "src/common/crc32.h"
#include "src/common/strutil.h"

#if OS_LINUX
#   include <pwd.h>
#   include <unistd.h>
#endif

static constexpr uint32_t kEN = Common::crc32("en");
static constexpr uint32_t kJA = Common::crc32("ja");
static constexpr uint32_t kDE = Common::crc32("de");
static constexpr uint32_t kFR = Common::crc32("fr");
static constexpr uint32_t kES = Common::crc32("es");
static constexpr uint32_t kIT = Common::crc32("it");
static constexpr uint32_t kKO = Common::crc32("ko");
static constexpr uint32_t kPL = Common::crc32("pl");
static constexpr uint32_t kZH = Common::crc32("zh");
static constexpr uint32_t kRU = Common::crc32("ru");
static constexpr uint32_t kCZ = Common::crc32("cz");
static constexpr uint32_t kHU = Common::crc32("hu");

namespace Common {

Language iso639ToLang(const std::string code) {
	switch (Common::crc32(code)) {
		case kEN: return kEnglish;
		case kJA: return kJapanese;
		case kDE: return kGerman;
		case kFR: return kFrench;
		case kES: return kSpanish;
		case kIT: return kItalian;
		case kKO: return kKorean;
		case kPL: return kPolish;
		case kZH: return kChinese;
		case kRU: return kRussian;
		case kCZ: return kCzech;
		case kHU: return kHungarian;
		default: return kUnrecognized;
	}
}

bool isX11() {
	return std::string(std::getenv("XDG_SESSION_TYPE")) == "x11";
}

bool isWayland() {
	return std::string(std::getenv("XDG_SESSION_TYPE")) == "wayland";
}

Language getSystemLanguage() {
#if OS_LINUX || OS_MACOS
	std::string lang = Common::toLower(std::locale("").name());

	// Check if the locale is a [language]_[locale].[encoding] triplet
	if (std::regex_match(lang, std::regex("[a-z]+(\\_[a-z]+)?(\\.[a-z0-9\\-]+)?"))) {
		const auto split = Common::split(lang, std::regex("\\_|\\."));

		const auto langCode = split[0];
		const auto localeCode = split[1];

		const auto language = iso639ToLang(langCode);

		if (language != kUnrecognized)
			return language;
	}
#endif

	// If no language is found or the language is not supported, always return english
	return kEnglish;
}

std::string getHomeDirectory() {
	std::string home = "";

#if OS_LINUX
	const char *homeEnv = std::getenv("HOME");
	home = std::string(homeEnv);
	if (home.empty()) {
		struct passwd *pwd = getpwuid(getuid());
		if (pwd)
			home = pwd->pw_dir;
		else
			home = ".";
	}
#elif OS_MACOS
#elif OS_WINDOWS
    home = std::getenv("USERPROFILE");
#endif
	return home;
}

std::string getUserDataDirectory() {
	std::string userData = "";
#if OS_LINUX
	const char *xdgUserData = std::getenv("XDG_DATA_HOME");
	if (!xdgUserData) {
		userData = getHomeDirectory();
		if (userData.empty())
			userData = ".";
		else
			userData += "/.local/share";
	} else {
		userData = xdgUserData;
	}
#elif OS_WINDOWS
    std::string appData = std::getenv("APPDATA");
    userData = appData;
#endif

	return userData;
}

VideoMode getPrimaryMonitorVideoMode() {
	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	if (!monitor)
		throw CreateException("Failed to get primary monitor");
	const GLFWvidmode *mode = glfwGetVideoMode(monitor);
	return VideoMode{
		mode->width, 
		mode->height, 
		mode->redBits + mode->greenBits + mode->blueBits,
		mode->refreshRate};
}

} // End of namespace Common
