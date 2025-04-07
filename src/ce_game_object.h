#pragma once

#include "ce_model.h"

//libs
#include <glm/gtc/matrix_transform.hpp>

//std
#include <memory>
#include <unordered_map>

struct Transform_2D_Component {
	glm::vec2 translation{};
	glm::vec2 scale{1.0f,1.0f};
	float rotation;
	glm::mat2 mat2() { 
		const float s = glm::sin(rotation);
		const float c = glm::cos(rotation);

		glm::mat2 rotation_matrix{ {c,s}, {-s,c} };
		glm::mat2 scale_mat{ {scale.x, 0.0f}, {0.0f,scale.y} };
		return rotation_matrix * scale_mat;
	};
};

struct Transform_3D_Component {
	glm::vec3 translation{};
	glm::vec3 scale{ 1.0f,1.0f,1.0f };
	glm::vec3 rotation;


	//matrix corresponds to translate * Ry * Rx * Rz * scale transform
	//rotation convention uses Tate-Bryan angles with Y(1),X(2),Z(3) axis order
	glm::mat4 mat4();
	glm::mat3 normal_matrix();
};

struct Point_Light_Component {
	float light_intensity{1.0f};
};

struct Transparency_Component {
	float alpha{1.0f};
};

class CE_Game_Object
{
public:
	using id_t = unsigned int;
	using map = std::unordered_map<id_t, CE_Game_Object>;

	static CE_Game_Object create_game_object() {
		static id_t current_id = 0;
		return CE_Game_Object{ current_id++ };
	};

	static CE_Game_Object create_point_light(float intensity = 10.0f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.0f));
	static CE_Game_Object create_transparent_object(float alpha = 1.0f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.0f));



	id_t get_id() const { return id; };

	CE_Game_Object(const CE_Game_Object&) = delete;
	CE_Game_Object& operator = (const CE_Game_Object&) = delete;

	CE_Game_Object(CE_Game_Object&&) = default;
	CE_Game_Object& operator = (CE_Game_Object&&) = default;

	
	glm::vec3 color{};
	Transform_3D_Component transform_3d{};

	//optional pointer components
	std::shared_ptr<CE_Model> model;
	std::unique_ptr<Point_Light_Component> point_light {nullptr};
	std::unique_ptr<Transparency_Component> transparency {nullptr};
private:
	CE_Game_Object(id_t object_id) : id{ object_id } {};


	id_t id;
};

