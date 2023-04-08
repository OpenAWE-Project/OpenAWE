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
#include <optional>

#include "src/common/types.h"

#include "src/platform/inputman.h"

#include "window.h"

namespace Platform {

Window::Window(ContextType type) {
	glfwDefaultWindowHints();

	switch (type) {
		case kOpenGL:
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
			glfwWindowHint(GLFW_SAMPLES, 4);
			glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
			break;

		case kOpenGLES:
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
			glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
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

	glfwSetCursorPos(_window, 960, 540);

	glfwSetInputMode(_window, GLFW_STICKY_KEYS, GLFW_TRUE);

	glfwSetWindowUserPointer(_window, this);

	InputMan.setKeyCallback(_window, &Window::callbackKey);
	glfwSetCursorPosCallback(_window, &Window::callbackMousePosition);
	InputMan.setMouseButtonCallback(_window, &Window::callbackMouseButton);
	glfwSetScrollCallback(_window, &Window::callbackMouseScroll);
	glfwSetCursorEnterCallback(_window, &Window::callbackMouseEnter);
	glfwSetFramebufferSizeCallback(_window, &Window::callbackFramebufferSize);
	InputMan.setGamepadButtonCallback(_window, &Window::callbackGamepadButton);
	InputMan.setGamepadTriggerCallback(_window, &Window::callbackGamepadTrigger);
	InputMan.setGamepadStickCallback(_window, &Window::callbackGamepadStick);
}

Window::~Window() {
	glfwDestroyWindow(_window);
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
	if (action == GLFW_REPEAT)
		return;

	Window *w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
	if (!w->_keyCallback)
		return;

	w->_keyCallback(key, scancode, action, mods);
}

void Window::callbackMousePosition(GLFWwindow *window, double xpos, double ypos) {
	Window *w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

	glm::vec2 absolute = glm::vec2(xpos, ypos);
	glm::vec2 delta = absolute - w->_lastMousePosition.value_or(absolute);

	if (w->_mousePositionCallback)
		w->_mousePositionCallback(absolute, delta);

	w->_lastMousePosition = absolute;
}

void Window::callbackMouseScroll(GLFWwindow *window, double xpos, double ypos) {
	Window *w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
	if (!w->_mouseScrollCallback)
		return;

	glm::vec2 position(xpos, ypos);
	w->_mouseScrollCallback(position, position);
}

void Window::callbackMouseEnter(GLFWwindow *window, int entered) {
	Window *w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
	
	if (entered) {
		double x, y;
		glfwGetCursorPos(w->_window, &x, &y);
		w->_lastMousePosition = glm::vec2(x, y);
	} else { // leaving the window
		w->_lastMousePosition = {};
	}

	if (!w->_mouseEnterCallback)
		return;

	w->_mouseEnterCallback(entered);
}

void Window::callbackMouseButton(GLFWwindow *window, int button, int action, int mods) {
	if (action == GLFW_REPEAT)
		return;

	Window *w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
	if (!w->_mouseButtonCallback)
		return;

	w->_mouseButtonCallback(button, action, mods);
}

void Window::callbackGamepadButton(GLFWwindow *window, int button, int action) {
	Window *w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
	if (!w->_gamepadButtonCallback)
		return;

	w->_gamepadButtonCallback(button, action);
}

void Window::callbackGamepadTrigger(GLFWwindow *window, int trigger, double pos) {
	Window *w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
	if (!w->_gamepadTriggerCallback)
		return;
	
	w->_gamepadTriggerCallback(trigger, pos, pos);
}

void Window::callbackGamepadStick(GLFWwindow *window, int stick, double xpos, double ypos) {
	Window *w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
	if (!w->_gamepadTriggerCallback)
		return;
	
	glm::vec2 vec(xpos, ypos);
	w->_gamepadStickCallback(stick, vec, vec);
}

void Window::callbackFramebufferSize(GLFWwindow *window, int width, int height) {
	Window *w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
}

void Window::setKeyCallback(const KeyCallback &keyCallback) {
	_keyCallback = keyCallback;
}

void Window::setMouseButtonCallback(const MouseButtonCallback &mouseButtonCallback) {
	_mouseButtonCallback = mouseButtonCallback;
}

void Window::setMousePositionCallback(const Axis2DCallback &mousePositionCallback) {
	_mousePositionCallback = mousePositionCallback;
}

void Window::setMouseScrollCallback(const Axis2DCallback &mouseScrollCallback) {
	_mouseScrollCallback = mouseScrollCallback;
}

void Window::setMouseEnterCallback(const MouseEnterCallback &mouseEnterCallback) {
	_mouseEnterCallback = mouseEnterCallback;
}

void Window::setGamepadButtonCallback(const GamepadButtonCallback &gamepadButtonCallback) {
	_gamepadButtonCallback = gamepadButtonCallback;
}

void Window::setGamepadStickCallback(const GamepadStickCallback &gamepadStickCallback) {
	_gamepadStickCallback = gamepadStickCallback;
}

void Window::setGamepadTriggerCallback(const GamepadTriggerCallback &GamepadTriggerCallback) {
	_gamepadTriggerCallback = GamepadTriggerCallback;
}

void Window::hideMouseCursor() {
	glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
}

void Window::showMouseCursor() {
	glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(_window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
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
