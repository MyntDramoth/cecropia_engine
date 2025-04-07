#include "keyboard_movement_controller.h"

void Keyboard_Movement_Controller::move_in_plane_XZ(GLFWwindow* window, float delta_time, CE_Game_Object& game_object) {
	glm::vec3 rotate{ 0 };

	if (glfwGetKey(window, keys.look_right) == GLFW_PRESS) { rotate.y += 1; }
	if (glfwGetKey(window, keys.look_left) == GLFW_PRESS) { rotate.y -= 1; }
	if (glfwGetKey(window, keys.look_up) == GLFW_PRESS) { rotate.x += 1; }
	if (glfwGetKey(window, keys.look_down) == GLFW_PRESS) { rotate.x -= 1; }

	if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
		game_object.transform_3d.rotation += look_speed * delta_time * glm::normalize(rotate);
	}

	//just to prevent the object from going upside down
	game_object.transform_3d.rotation.x = glm::clamp(game_object.transform_3d.rotation.x, -1.5f, 1.5f);
	//just to prevent repeated turning in one direction
	game_object.transform_3d.rotation.y = glm::mod(game_object.transform_3d.rotation.y, glm::two_pi<float>());

	float yaw = game_object.transform_3d.rotation.y;
	const glm::vec3 forward_dir{sin(yaw),0.0f,cos(yaw)};
	const glm::vec3 right_dir{ forward_dir.z, 0.0f, -forward_dir.x};
	const glm::vec3 up_dir{0.0f,-1.0f,0.0f};

	glm::vec3 move_dir{ 0.0f };


	if (glfwGetKey(window, keys.move_forward) == GLFW_PRESS) { move_dir += forward_dir; }
	if (glfwGetKey(window, keys.move_backward) == GLFW_PRESS) { move_dir -= forward_dir; }
	if (glfwGetKey(window, keys.move_right) == GLFW_PRESS) { move_dir += right_dir; }
	if (glfwGetKey(window, keys.move_left) == GLFW_PRESS) { move_dir -= right_dir; }
	if (glfwGetKey(window, keys.move_up) == GLFW_PRESS) { move_dir += up_dir; }
	if (glfwGetKey(window, keys.move_down) == GLFW_PRESS) { move_dir -= up_dir; }

	if (glm::dot(move_dir, move_dir) > std::numeric_limits<float>::epsilon()) {
		game_object.transform_3d.translation += move_speed * delta_time * glm::normalize(move_dir);
	}
}