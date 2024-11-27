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

#include <spdlog/spdlog.h>
#include <imgui_impl_glfw.h>

#include "src/common/types.h"

#include "src/platform/gamepadman.h"
#include "src/platform/window.h"

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
	_contentScale = glm::vec2(1, 1);
	_desiredResolution = glm::vec2(1920, 1080);

	glfwSetInputMode(_window, GLFW_STICKY_KEYS, GLFW_TRUE);

	glfwSetWindowUserPointer(_window, this);

	glfwSetKeyCallback(_window, &Window::callbackKey);
	glfwSetCharCallback(_window, &Window::callbackChar);
	glfwSetCursorPosCallback(_window, &Window::callbackMousePosition);
	glfwSetMouseButtonCallback(_window, &Window::callbackMouseButton);
	glfwSetScrollCallback(_window, &Window::callbackMouseScroll);
	glfwSetCursorEnterCallback(_window, &Window::callbackMouseEnter);
	glfwSetFramebufferSizeCallback(_window, &Window::callbackFramebufferSize);
	glfwSetWindowFocusCallback(_window, &Window::callbackWindowsFocusCallback);

	IMGUI_CHECKVERSION();
	_imguiCtx = ImGui::CreateContext();

	switch (type) {
		case kOpenGL: [[fallthrough]];
		case kOpenGLES:
			ImGui_ImplGlfw_InitForOpenGL(_window, false);
			break;

		case kVulkan:
			ImGui_ImplGlfw_InitForVulkan(_window, false);
			break;

		default:
			ImGui_ImplGlfw_InitForOther(_window, false);
			break;
	}
}

Window::~Window() {
	ImGui_ImplGlfw_Shutdown();
	glfwDestroyWindow(_window);
}

void Window::makeCurrent() {
	glfwMakeContextCurrent(_window);
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(_imguiCtx));
	ImGui_ImplGlfw_NewFrame();
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
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

	if (action == GLFW_REPEAT)
		return;

	Window *w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
	if (!w->_keyCallback)
		return;

	w->_keyCallback(key, scancode, action, mods);
}

void Window::callbackChar(GLFWwindow *window, unsigned int c) {
	ImGui_ImplGlfw_CharCallback(window, c);
}

void Window::callbackMousePosition(GLFWwindow *window, double xpos, double ypos) {
	ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);

	Window *w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

	glm::vec2 absolute = glm::vec2(xpos, ypos);
	glm::vec2 delta = absolute - w->_lastMousePosition.value_or(absolute);

	if (w->_mousePositionCallback)
		w->_mousePositionCallback(absolute, delta);

	w->_lastMousePosition = absolute;
}

void Window::callbackMouseScroll(GLFWwindow *window, double xpos, double ypos) {
	ImGui_ImplGlfw_ScrollCallback(window, xpos, ypos);

	Window *w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
	if (!w->_mouseScrollCallback)
		return;

	glm::vec2 position(xpos, ypos);
	w->_mouseScrollCallback(position, position);
}

void Window::callbackMouseEnter(GLFWwindow *window, int entered) {
	ImGui_ImplGlfw_CursorEnterCallback(window, entered);

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
	ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

	if (action == GLFW_REPEAT)
		return;

	Window *w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
	if (!w->_mouseButtonCallback)
		return;

	w->_mouseButtonCallback(button, action, mods);
}

void Window::callbackWindowsFocusCallback(GLFWwindow *window, int focused) {
	ImGui_ImplGlfw_WindowFocusCallback(window, focused);
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

bool Window::isMouseCursorVisible() {
	return glfwGetInputMode(_window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL;
}

void Window::setMouseCursorVisible(bool visible) {
	if (visible)
		glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	else
		glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Window::setRawMouseMotion(bool enabled) {
	if (!glfwRawMouseMotionSupported()) {
		spdlog::warn("Raw mouse motion is not supported");
		return;
	}
	
	if (enabled)	
		glfwSetInputMode(_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	else
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

glm::vec2 Window::getSize() {
	unsigned int width, height;
	glfwGetFramebufferSize(_window, reinterpret_cast<int *>(&width), reinterpret_cast<int *>(&height));
	return glm::vec2(width, height);
}

void Window::setSize(unsigned int width, unsigned int height) {
	_desiredResolution = glm::vec2(width, height);
	glm::vec2 oldResolution = getSize();
	glfwSetWindowSize(_window, width, height);
	// On Wayland, when window is fullscreen, the line above does nothing.
	// That means we have to scale the content ourselves...
	if (getSize() == oldResolution) {
		_contentScale = oldResolution / _desiredResolution;
	}
}

glm::vec2 Window::getContentScale() {
	return _contentScale;
}

void Window::setFullscreen(bool fullscreen) {
	unsigned int width, height;
	getSize(width, height);
	glfwSetWindowMonitor(_window, fullscreen ? glfwGetPrimaryMonitor() : NULL, 0, 0, width, height, GLFW_DONT_CARE);
}

}
