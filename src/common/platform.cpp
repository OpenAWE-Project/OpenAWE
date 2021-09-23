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

#include "src/common/platform.h"
#include "src/common/types.h"

#if OS_LINUX
#   include <pwd.h>
#   include <unistd.h>
#endif

namespace Common {

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
#endif
	return home;
}

std::string getConfigDirectory() {
#if OS_LINUX
	return std::getenv("XDG_CONFIG_HOME");
#endif

	return ".";
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
#endif

	return userData;
}

}
