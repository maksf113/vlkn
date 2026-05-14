#include "window/InputManager.hpp"

void InputManager::setCallbacks(GLFWwindow* win)
{
	m_window = win;
	glfwSetWindowUserPointer(win, this);
	// Setting callbacks
	glfwSetWindowSizeCallback(win, [](GLFWwindow* win, int width, int height)
		{auto im = static_cast<InputManager*>(glfwGetWindowUserPointer(win));
	im->dispatchResizeEvent(static_cast<uint32_t>(width), static_cast<uint32_t>(height)); });
	glfwSetKeyCallback(win, [](GLFWwindow* win, int key, int scancode, int action, int mods)
		{auto im = static_cast<InputManager*>(glfwGetWindowUserPointer(win));
	im->dispatchKeyEvent(key, scancode, action, mods); });
	glfwSetMouseButtonCallback(win, [](GLFWwindow* win, int button, int action, int mods) {
		auto im = static_cast<InputManager*>(glfwGetWindowUserPointer(win));
		im->dispatchMouseButtonEvent(button, action, mods);
		});
	glfwSetCursorPosCallback(win, [](GLFWwindow* win, double x, double y) {
		auto im = static_cast<InputManager*>(glfwGetWindowUserPointer(win));
		im->dispatchMousePositionEvent(x, y);
		});
	glfwSetScrollCallback(win, [](GLFWwindow* win, double x, double y) {
		auto im = static_cast<InputManager*>(glfwGetWindowUserPointer(win));
		im->dispatchMouseWheelEvent(x, y);
		});
}

void InputManager::addReceiver(std::shared_ptr<InputReceiver> receiver)
{
	m_receivers.push_back(receiver);
}

void InputManager::dispatchKeyEvent(int key, int scancode, int action, int mods)
{
	//ImGui_ImplGlfw_KeyCallback(m_window, key, scancode, action, mods);
	//ImGuiIO& io = ImGui::GetIO();
	//if (io.WantCaptureKeyboard)
	//	return;
	setKeyState(key, action);
	for (const auto& weakReceiver : m_receivers)
	{
		if (auto sharedReceiver = weakReceiver.lock())
		{
			sharedReceiver->keyEvent(key, scancode, action, mods);
		}
	}
}

void InputManager::dispatchMouseButtonEvent(int button, int action, int mods)
{
	//ImGui_ImplGlfw_MouseButtonCallback(m_window, button, action, mods);
	//ImGuiIO& io = ImGui::GetIO();
	//if (io.WantCaptureMouse)
	//	return;
	setMouseButtonState(button, action);
	for (const auto& weakReceiver : m_receivers)
	{
		if (auto sharedReceiver = weakReceiver.lock())
		{
			sharedReceiver->mouseButtonEvent(button, action, mods);
		}
	}
}

void InputManager::dispatchMousePositionEvent(double x, double y)
{
	//ImGuiIO& io = ImGui::GetIO();
	//if (io.WantCaptureMouse)
	//{
	//	m_firstCursorMove = true;
	//	return;
	//}
	if (m_firstCursorMove)
	{
		m_cursorLastX = x;
		m_cursorLastY = y;
		m_firstCursorMove = false;
	}
	m_cursorDX = x - m_cursorLastX;
	m_cursorDY = m_cursorLastY - y;
	m_cursorLastX = x;
	m_cursorLastY = y;
	for (const auto& weakReceiver : m_receivers)
	{
		if (auto sharedReceiver = weakReceiver.lock())
		{
			sharedReceiver->mousePositionEvent(x, y);
		}
	}
}

void InputManager::dispatchMouseWheelEvent(double x, double y)
{
	for (const auto& weakReceiver : m_receivers)
	{
		if (auto sharedReceiver = weakReceiver.lock())
		{
			sharedReceiver->mouseWheelEvent(x, y);
		}
	}
}

void InputManager::dispatchResizeEvent(uint32_t width, uint32_t height)
{
	for (const auto& weakReceiver : m_receivers)
	{
		if (auto sharedReceiver = weakReceiver.lock())
		{
			sharedReceiver->resizeEvent(width, height);
		}
	}
}

bool InputManager::isKeyPressed(int key) const
{
	if (key >= 0 && key < m_keyStates.size())
		return m_keyStates[key];
	else
		return false;
}

bool InputManager::isMouseButtonPressed(int button) const
{
	if (button >= 0 && button < m_mouseButtonStates.size())
		return m_mouseButtonStates[button];
	else
		return false;
}

double InputManager::cursorDX() const
{
	return m_cursorDX;
}

double InputManager::cursorDY() const
{
	return m_cursorDY;
}

void InputManager::endFrame()
{
	m_cursorDX = 0.0;
	m_cursorDY = 0.0;
}

void InputManager::setKeyState(int key, int action)
{
	if (key >= 0 && key < m_keyStates.size())
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			m_keyStates[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			m_keyStates[key] = false;
		}
	}
}
void InputManager::setMouseButtonState(int button, int action)
{
	if (button >= 0 && button < m_mouseButtonStates.size())
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			m_mouseButtonStates[button] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			m_mouseButtonStates[button] = false;
		}
	}
}