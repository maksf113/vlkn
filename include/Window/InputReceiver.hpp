#pragma once
#include <stdint.h>

class InputReceiver
{
public:
	virtual ~InputReceiver() = default;

	virtual bool keyEvent(int key, int scancode, int action, int mods) { return false; }
	virtual bool mouseButtonEvent(int button, int action, int mods) { return false; }
	virtual bool mousePositionEvent(double x, double y) { return false; }
	virtual bool mouseWheelEvent(double x, double y) { return false; }
	virtual bool resizeEvent(uint32_t width, uint32_t height) { return false; }
};