#pragma once
#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif
#include <GLFW/glfw3.h>

#include <string>

class Window
{
private:
	GLFWwindow* m_handle;
	uint32_t m_width;
	uint32_t m_height;
	std::string m_title;
public:
	Window(uint32_t width, uint32_t height, std::string_view title);
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	Window(Window&& other) noexcept;
	Window& operator=(Window&& other) noexcept;
	~Window();

	GLFWwindow* get() const;
	operator GLFWwindow*() const;

	void pollEvents() const;
	bool shouldClose() const;
	uint32_t width() const;
	uint32_t height() const;
	void setWidth(uint32_t width);
	void setHeight(uint32_t height);
};