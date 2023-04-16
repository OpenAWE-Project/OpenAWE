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
#include <optional>
#include <functional>

#define GLFW_INCLUDE_NONE
#ifdef WITH_VULKAN
#   define GLFW_INCLUDE_VULKAN
#endif // WITH_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "glcontext.h"
#include "vulkancontext.h"

namespace Platform {

typedef std::function<void (int key, int scancode, int action, int mods)> KeyCallback;
typedef std::function<void (int button, int action, int mods)> MouseButtonCallback;
typedef std::function<void (glm::vec2 absolute, glm::vec2 delta)> Axis2DCallback;
typedef std::function<void (int entered)> MouseEnterCallback;

class Window : public GLContext, public VulkanContext {
public:
	enum ContextType {
		kOpenGL,
		kOpenGLES,
		kVulkan
	};

	Window(ContextType type);
	~Window();

	void makeCurrent() override;
	void swap() override;

	void getSize(unsigned int &width, unsigned int &height) override;
	glm::vec2 getSize() override;
	void setSize(unsigned int width, unsigned int height) override;
	void setFullscreen(bool fullscreen) override;

	bool isMouseCursorVisible();
	void setMouseCursorVisible(bool visible);
	void setRawMouseMotion(bool enabled);

#ifdef WITH_VULKAN
	const char **getInstanceExtensions(unsigned int &numExtensions) override;
	void createWindowSurface(VkInstance &instance, VkSurfaceKHR &surface) override;
#endif // WITH_VULKAN

	void setTitle(const std::string &title);
	bool shouldClose();

	void setKeyCallback(const KeyCallback &keyCallback);
	void setMouseButtonCallback(const MouseButtonCallback &mouseButtonCallback);
	void setMousePositionCallback(const Axis2DCallback &mousePositionCallback);
	void setMouseScrollCallback(const Axis2DCallback &mouseScrollCallback);
	void setMouseEnterCallback(const MouseEnterCallback &mouseEnterCallback);

	GLFWwindow * getWindowHandle();

private:
	static void callbackKey(GLFWwindow *window, int key, int scancode, int action, int mods);
	static void callbackChar(GLFWwindow *window, unsigned int c);
	static void callbackMousePosition(GLFWwindow *window, double xpos, double ypos);
	static void callbackMouseScroll(GLFWwindow *window, double xpos, double ypos);
	static void callbackMouseButton(GLFWwindow *window, int button, int action, int mods);
	static void callbackMouseEnter(GLFWwindow *window, int entered);

	static void callbackWindowsFocusCallback(GLFWwindow *window, int focused);

	static void callbackFramebufferSize(GLFWwindow *window, int width, int height);

	GLFWwindow *_window;
	void *_imguiCtx;

	KeyCallback _keyCallback;
	MouseButtonCallback _mouseButtonCallback;
	Axis2DCallback _mousePositionCallback;
	Axis2DCallback _mouseScrollCallback;
	MouseEnterCallback _mouseEnterCallback;
	std::optional<glm::vec2> _lastMousePosition;
};

} // End of namespace Graphics

#endif //AWE_WINDOW_H
