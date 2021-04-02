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

#ifndef AWE_PLATFORM_H
#define AWE_PLATFORM_H


namespace Graphics {

/*!
 * \brief class for handling platform specific stuff
 *
 * This class handles the global tear up and tear down
 * of platform specific stuff contained in glfw
 */
class Platform {
public:
	Platform();

	void init();
	void terminate();

	void update();

private:
	static void errorCallback(int code, const char *description);
};

}

#endif //AWE_PLATFORM_H
