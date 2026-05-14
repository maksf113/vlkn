#pragma once
#include "window/InputReceiver.hpp"

#include <GLFW/glfw3.h>
//#include <imgui_impl_glfw.h>

#include <memory>
#include <vector>
#include <array>

class InputManager
{
private:
	std::vector<std::weak_ptr<InputReceiver>> m_receivers;
	GLFWwindow* m_window = nullptr;
	std::array<bool, GLFW_KEY_LAST + 1> m_keyStates{ false };
	std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> m_mouseButtonStates{ false };
	double m_cursorLastX;
	double m_cursorLastY;
	double m_cursorDX = 0.0;
	double m_cursorDY = 0.0;
	bool m_firstCursorMove = true;

public:
	void setCallbacks(GLFWwindow* window);
	void addReceiver(std::shared_ptr<InputReceiver> receiver);
	void dispatchKeyEvent(int key, int scancode, int action, int mods);
	void dispatchMouseButtonEvent(int button, int action, int mods);
	void dispatchMousePositionEvent(double x, double y);
	void dispatchMouseWheelEvent(double x, double y);
	void dispatchResizeEvent(uint32_t width, uint32_t height);
	bool isKeyPressed(int key) const;
	bool isMouseButtonPressed(int button) const;
	double cursorDX() const;
	double cursorDY() const;
	void endFrame();
private:
	void setKeyState(int key, int action);
	void setMouseButtonState(int button, int action);
};
