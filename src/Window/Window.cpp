#include "window/Window.hpp"

#include <stdexcept>

Window::Window(uint32_t width, uint32_t height, std::string_view title) :
    m_width(width), m_height(height), m_title(title)
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    m_handle = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), m_title.c_str(), nullptr, nullptr);
}

Window::Window(Window&& other) noexcept :
    m_handle(other.m_handle), m_width(other.m_width),
	m_height(other.m_height), m_title(std::move(other.m_title))
{
	other.m_handle = nullptr;
}

Window& Window::operator=(Window&& other) noexcept
{
	m_handle = other.m_handle;
	m_width = other.m_width;
	m_height = other.m_height;
	m_title = std::move(other.m_title);
	other.m_handle = nullptr;
	return *this;
}

Window::~Window()
{
    if(m_handle != nullptr)
    {
		glfwDestroyWindow(m_handle);
    }
}

GLFWwindow* Window::get() const
{
    return m_handle;
}

Window::operator GLFWwindow* () const
{
    return m_handle;
}

void Window::pollEvents() const
{
    glfwPollEvents();
}

bool Window::shouldClose() const
{
    return glfwWindowShouldClose(m_handle);
}

uint32_t Window::getWidth() const
{
    return m_width;
}

uint32_t Window::getHeight() const
{
    return m_height;
}

void Window::setWidth(uint32_t width)
{
    m_width = width;
}

void Window::setHeight(uint32_t height)
{
    m_height = height;
}   
