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

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include "platform.h"

namespace Graphics {

Platform::Platform() {
	glfwSetErrorCallback(&Platform::errorCallback);
}

void Platform::init() {
	spdlog::info("GLFW Info: {}", glfwGetVersionString());

	int result = glfwInit();
	if (result == GLFW_FALSE)
		throw std::runtime_error("Failed to initialize glfw");
}

void Platform::terminate() {
	glfwTerminate();
}

void Platform::update() {
	glfwPollEvents();
}

void Platform::errorCallback(int code, const char *description) {
	spdlog::error("GLFW Error {}: {}", code, description);
}

}