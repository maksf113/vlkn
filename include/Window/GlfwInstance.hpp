#pragma once

class GlfwInstance
{
public:
	GlfwInstance();
	GlfwInstance(const GlfwInstance&) = delete;
	GlfwInstance& operator=(const GlfwInstance&) = delete;
	GlfwInstance(GlfwInstance&& other) noexcept = default;
	GlfwInstance& operator=(GlfwInstance&& other) noexcept = default;
	~GlfwInstance();
};