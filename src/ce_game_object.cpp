#include "ce_game_object.h"

glm::mat4 Transform_3D_Component::mat4() {
	
	const float c3 = glm::cos(rotation.z);
	const float s3 = glm::sin(rotation.z);
	const float c2 = glm::cos(rotation.x);
	const float s2 = glm::sin(rotation.x);
	const float c1 = glm::cos(rotation.y);
	const float s1 = glm::sin(rotation.y);
	return glm::mat4{
		{
			scale.x * (c1 * c3 + s1 * s2 * s3),
			scale.x * (c2 * s3),
			scale.x * (c1 * s2 * s3 - c3 * s1),
			0.0f,
		},
		{
			scale.y * (c3 * s1 * s2 - c1 * s3),
			scale.y * (c2 * c3),
			scale.y * (c1 * c3 * s2 + s1 * s3),
			0.0f,
		},
		{
			scale.z * (c2 * s1),
			scale.z * (-s2),
			scale.z * (c1 * c2),
			0.0f,
		},
		{translation.x, translation.y, translation.z, 1.0f} };
	
}

glm::mat3 Transform_3D_Component::normal_matrix() {
	const float c3 = glm::cos(rotation.z);
	const float s3 = glm::sin(rotation.z);
	const float c2 = glm::cos(rotation.x);
	const float s2 = glm::sin(rotation.x);
	const float c1 = glm::cos(rotation.y);
	const float s1 = glm::sin(rotation.y);

	const glm::vec3 inverse_scale = 1.0f / scale;
	return glm::mat3{
		{
			inverse_scale.x * (c1 * c3 + s1 * s2 * s3),
			inverse_scale.x * (c2 * s3),
			inverse_scale.x * (c1 * s2 * s3 - c3 * s1),

		},
		{
			inverse_scale.y * (c3 * s1 * s2 - c1 * s3),
			inverse_scale.y * (c2 * c3),
			inverse_scale.y * (c1 * c3 * s2 + s1 * s3),

		},
		{
			inverse_scale.z * (c2 * s1),
			inverse_scale.z * (-s2),
			inverse_scale.z * (c1 * c2),

		}
	};
	
}

CE_Game_Object CE_Game_Object::create_point_light(float intensity, float radius, glm::vec3 color) {
		CE_Game_Object game_object = CE_Game_Object::create_game_object();
		game_object.color = color;
		game_object.transform_3d.scale.x = radius;
		game_object.point_light = std::make_unique<Point_Light_Component>();
		game_object.point_light->light_intensity = intensity;
		return game_object;
}

CE_Game_Object CE_Game_Object::create_transparent_object(float alpha, float radius, glm::vec3 color) {
	CE_Game_Object game_object = CE_Game_Object::create_game_object();
	game_object.color = color;
	game_object.transform_3d.scale.x = radius;
	game_object.transparency = std::make_unique<Transparency_Component>();
	game_object.transparency->alpha = alpha;
	return game_object;
}