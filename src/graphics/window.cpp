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

#include <stdexcept>

#include "src/common/types.h"

#include "src/graphics/window.h"

namespace Graphics {

Window::Window(ContextType type) {
	glfwDefaultWindowHints();

	switch (type) {
		case kOpenGL:
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
			glfwWindowHint(GLFW_SAMPLES, 4);
			break;

		case kVulkan:
			if (!glfwVulkanSupported())
				throw std::runtime_error("Vulkan not supported on this system");

			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			break;
	}

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	_window = glfwCreateWindow(1920, 1080, "", nullptr, nullptr);

	glfwSetInputMode(_window, GLFW_STICKY_KEYS, GLFW_TRUE);

	glfwSetWindowUserPointer(_window, this);

	glfwSetKeyCallback(_window, &Window::callbackKey);
	glfwSetCursorPosCallback(_window, &Window::callbackMousePosition);
	glfwSetMouseButtonCallback(_window, &Window::callbackMouseButton);
	glfwSetFramebufferSizeCallback(_window, &Window::callbackFramebufferSize);
}

void Window::makeCurrent() {
	glfwMakeContextCurrent(_window);
}

void Window::swap() {
	glfwSwapBuffers(_window);
}

void Window::setTitle(const std::string &title) {
	glfwSetWindowTitle(_window, title.c_str());
}

bool Window::shouldClose() {
	return glfwWindowShouldClose(_window);
}

void Window::callbackKey(GLFWwindow *window, int key, int scancode, int action, int mods) {
	Window *w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
	if (!w->_keyCallback)
		return;

	w->_keyCallback(key, scancode, action, mods);
}

void Window::callbackMousePosition(GLFWwindow *window, double xpos, double ypos) {
	Window *w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
}

void Window::callbackMouseButton(GLFWwindow *window, int button, int action, int mods) {
	Window *w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
}

void Window::callbackFramebufferSize(GLFWwindow *window, int width, int height) {
	Window *w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
}

void Window::setKeyCallback(const KeyCallback &keyCallback) {
	_keyCallback = keyCallback;
}

GLFWwindow * Window::getWindowHandle() {
	return _window;
}

#ifdef WITH_VULKAN

const char ** Window::getInstanceExtensions(unsigned int &numExtensions) {
	return glfwGetRequiredInstanceExtensions(&numExtensions);
}

void Window::createWindowSurface(VkInstance &instance, VkSurfaceKHR &surface) {
	VkResult result = glfwCreateWindowSurface(instance, _window, nullptr, &surface);
	if (result != VK_SUCCESS)
		throw std::runtime_error("Error creating window surface");
}

#endif

void Window::getSize(unsigned int &width, unsigned int &height) {
	glfwGetFramebufferSize(_window, reinterpret_cast<int *>(&width), reinterpret_cast<int *>(&height));
}

}
