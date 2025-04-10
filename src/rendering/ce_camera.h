#pragma once

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm/glm.hpp>

class CE_Camera
{
public:
	void set_orthographic_projection(float left, float right, float top, float bottom, float near, float far);
	void set_perspective_projection(float fov_y, float aspect_ratio, float near, float far);

	void set_view_direction(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{0.0f,-1.0f,0.0f});
	void set_view_target(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{ 0.0f,-1.0f,0.0f });
	void set_view_YXZ(glm::vec3 position, glm::vec3 rotation);

	const glm::mat4& get_projection() const { return projection_matrix; };
	const glm::mat4& get_view() const { return view_matrix; };
	const glm::mat4& get_inverse_view() const { return inverse_view_matrix; };
	const glm::vec3 get_position() const {return glm::vec3(inverse_view_matrix[3]); };

private:
	glm::mat4 projection_matrix{ 1.0f };
	glm::mat4 view_matrix{ 1.0f };
	glm::mat4 inverse_view_matrix{ 1.0f };
};

