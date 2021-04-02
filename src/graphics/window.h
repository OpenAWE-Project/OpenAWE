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

#ifndef AWE_WINDOW_H
#define AWE_WINDOW_H

#include <string>
#include <functional>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "src/graphics/glcontext.h"
#include "src/graphics/vulkancontext.h"

namespace Graphics {

typedef std::function<void (int key, int scancode, int action, int mods)> KeyCallback;

class Window : public GLContext, public VulkanContext {
public:
	enum ContextType {
		kOpenGL,
		kVulkan
	};

	Window(ContextType type);

	void makeCurrent() override;
	void swap() override;

	void getSize(unsigned int &width, unsigned int &height) override;

	const char **getInstanceExtensions(unsigned int &numExtensions) override;

	void createWindowSurface(VkInstance &instance, VkSurfaceKHR &surface) override;

	void setTitle(const std::string &title);
	bool shouldClose();

	void setKeyCallback(const KeyCallback &keyCallback);

	GLFWwindow * getWindowHandle();

private:
	static void callbackKey(GLFWwindow *window, int key, int scancode, int action, int mods);
	static void callbackMousePosition(GLFWwindow *window, double xpos, double ypos);
	static void callbackMouseButton(GLFWwindow *window, int button, int action, int mods);

	static void callbackFramebufferSize(GLFWwindow *window, int width, int height);

	GLFWwindow *_window;

	KeyCallback _keyCallback;
};

} // End of namespace Graphics

#endif //AWE_WINDOW_H
