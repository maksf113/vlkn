#pragma once
#include "window/InputManager.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace vk
{
	class Camera
	{
	private:
		float m_aspectRatio = 16.0f / 9.0f;


		// 3D
		glm::vec3 m_pos;
		glm::vec3 m_target = glm::vec3(0.0f);
		float m_distToTarget = 3.2f;

		float m_near3D = 0.05f;
		float m_far3D = 100.0f;
		float m_fov = 45.0f;

		glm::vec3 m_viewDir = glm::vec3(0.0f);
		glm::vec3 m_rightDir = glm::vec3(0.0f);
		glm::vec3 m_upDir = glm::vec3(0.0f);
		glm::vec3 m_worldUp = glm::vec3(0.0f, 0.0f, 1.0f);
		float m_azimuth = -45.0f;
		float m_elevation = 30.0f;
		float m_orbitSensitivity = 0.2f;
		float m_zoomSensitivity = 0.05f;
	public:
		Camera();
		~Camera() = default;
		Camera(const Camera&) = default;
		Camera(Camera&&) = default;
		Camera& operator=(const Camera&) = default;
		Camera& operator=(Camera&&) = default;
		glm::mat4 view() const;
		glm::mat4 projection() const;
		glm::vec3 target() const;
		glm::vec3 viewDir() const;
		glm::vec3 pos() const;
		void setAspectRatio(uint32_t width, uint32_t height);
		void processInput(const InputManager& im);
		void mouseWheelEvent(double x, double y);
		void zoom(float y);
		void orbit(double x, double y);

	//private:
		void updateVectors3D();
	};
}


