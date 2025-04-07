#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 frag_color;
layout(location = 1) out vec3 frag_pos_world;
layout(location = 2) out vec3 frag_normal_world;
layout(location = 3) out vec2 frag_uv;

struct Point_Lights {
	vec4 position; //ignore w
	vec4 color;
};

layout(push_constant) uniform Push {
    mat4 model_matrix;
    mat4 normal_matrix;
} push; 

layout(set = 0, binding = 0) uniform Global_UBO {
    mat4 projection_matrix;
    mat4 view_matrix;
     mat4 inverse_view_matrix;
    vec4 ambient_light_color; 
    Point_Lights point_lights[10];
    int num_lights;
} ubo;

void main() {

    vec4 position_world = push.model_matrix * vec4(position,1.0);
    gl_Position = ubo.projection_matrix * ubo.view_matrix * position_world;
    frag_normal_world = normalize(mat3(push.normal_matrix) * normal);
    frag_pos_world = position_world.xyz;
    frag_color = color;
    frag_uv = uv;
}