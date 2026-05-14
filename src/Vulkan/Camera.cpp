#include "vulkan/Camera.hpp"

namespace vk
{
	Camera::Camera()
	{
		updateVectors3D();
	}

	glm::mat4 Camera::view() const
	{
		return glm::lookAt(m_pos, m_target, m_upDir);
	}

	glm::mat4 Camera::projection() const
	{
		{
			return glm::perspective(glm::radians(m_fov), m_aspectRatio, m_near3D, m_far3D);
		}
	}

	glm::vec3 Camera::target() const
	{
		return m_target;
	}

	glm::vec3 Camera::viewDir() const
	{
		return m_viewDir;
	}

	glm::vec3 Camera::pos() const
	{
		return m_pos;
	}

	void Camera::zoom(float y)
	{
		m_distToTarget -= y * m_distToTarget * m_zoomSensitivity;
		if (m_distToTarget < 0.05f)
			m_distToTarget = 0.05f;
		//updateVectors3D();
	}

	void Camera::orbit(double x, double y)
	{
		m_azimuth -= static_cast<float>(x) * m_orbitSensitivity;
		m_elevation += static_cast<float>(y) * m_orbitSensitivity;
		if (m_azimuth < 0.0f)
		{
			m_azimuth += 360.0f;
		}
		else if (m_azimuth >= 360.0f)
		{
			m_azimuth -= 360.0f;
		}

		if (m_elevation > 89.0)
		{
			m_elevation = 89.0f;
		}
		else if (m_elevation < -89.0)
		{
			m_elevation = -89.0f;
		}
		//updateVectors3D();
	}

	void Camera::updateVectors3D()
	{
		float azimRad = glm::radians(m_azimuth);
		float elevRad = glm::radians(m_elevation);

		float sinAzim = sin(azimRad);
		float cosAzim = cos(azimRad);
		float sinElev = sin(elevRad);
		float cosElev = cos(elevRad);

		m_pos = m_target + m_distToTarget * glm::vec3(cosAzim * cosElev, cosElev * sinAzim, sinElev);
		m_viewDir = glm::normalize(m_target - m_pos);
		m_rightDir = glm::normalize(glm::cross(m_viewDir, m_worldUp));
		m_upDir = glm::normalize(glm::cross(m_rightDir, m_viewDir));
	}


	void Camera::setAspectRatio(uint32_t width, uint32_t height)
	{
		m_aspectRatio = static_cast<float>(width) / height;
	}

	void Camera::processInput(const InputManager& im)
	{
		if (im.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
		{
			orbit(im.cursorDX(), im.cursorDY());
		}
	}

	void Camera::mouseWheelEvent(double x, double y)
	{
		zoom(y);
	}
}