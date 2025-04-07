#version 450

layout(location=0) in vec2 frag_offset;
layout(location=0) out vec4 frag_color;

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
    float dist = sqrt(dot(frag_offset,frag_offset));
    if(dist >= 1.0) {
        discard;
    }
    frag_color = vec4(push.color.xyz,1.0);
}