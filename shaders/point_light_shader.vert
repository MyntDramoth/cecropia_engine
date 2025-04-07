#version 450

const vec2 OFFSETS[6] = vec2[](
  vec2(-1.0, -1.0),
  vec2(-1.0, 1.0),
  vec2(1.0, -1.0),
  vec2(1.0, -1.0),
  vec2(-1.0, 1.0),
  vec2(1.0, 1.0)
);

layout(location=0) out vec2 frag_offset;

struct Point_Lights {
	vec4 position; //ignore w
	vec4 color;
};

layout(set = 0, binding = 0) uniform Global_UBO {
    mat4 projection_matrix;
    mat4 view_matrix;
    mat4 inverse_view_matrix;
    vec4 ambient_light_color; 
    Point_Lights point_lights[10];
    int num_lights;
} ubo;

layout(push_constant) uniform Push {
  vec4 position; //ignore w
	vec4 color;
  float radius;
}push;


void main() {
    frag_offset = OFFSETS[gl_VertexIndex];
    vec3 camera_right_world = {ubo.view_matrix[0][0],ubo.view_matrix[1][0],ubo.view_matrix[2][0]};
    vec3 camera_up_world = {ubo.view_matrix[0][1],ubo.view_matrix[1][1],ubo.view_matrix[2][1]};

    vec3 world_pos = push.position.xyz 
    + push.radius * frag_offset.x * camera_right_world
    + push.radius * frag_offset.y * camera_up_world;

    gl_Position = ubo.projection_matrix * ubo.view_matrix * vec4(world_pos,1.0);

}