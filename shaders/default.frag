#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 frag_color;
layout(location = 1) in vec3 frag_pos_world;
layout(location = 2) in vec3 frag_normal_world;
layout(location = 3) in vec2 frag_uv;

layout(location = 0) out vec4 outColor;

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

layout(binding = 1) uniform sampler2D tex;

void main() {
    vec3 diffuse_light = ubo.ambient_light_color.xyz * ubo.ambient_light_color.w;
    vec3 specular_light = vec3(0.0);
    vec3 surface_normal = normalize(frag_normal_world);
    vec3 camera_world_pos = ubo.inverse_view_matrix[3].xyz;
    vec3 view_direction = normalize(camera_world_pos - frag_pos_world);

    for(int i = 0; i<ubo.num_lights; i++) {
        Point_Lights light = ubo.point_lights[i];
        vec3 direction_to_light = light.position.xyz - frag_pos_world;
        float attenuation = 1.0 / dot(direction_to_light,direction_to_light); //distance squared
        direction_to_light = normalize(direction_to_light);
        float cos_angle_of_incidence = max(dot(surface_normal, direction_to_light),0);
        vec3 light_intensity = light.color.xyz * light.color.w * attenuation;
        diffuse_light += light_intensity * cos_angle_of_incidence;

        //specular lighting
        vec3 half_angle = normalize(direction_to_light + view_direction);
        float blinn_term = dot(surface_normal,half_angle);
        blinn_term = clamp(blinn_term,0,1);
        blinn_term = pow(blinn_term,32.0); //higher power is a sharper highlight
        specular_light += light_intensity * blinn_term;
    } 
    
    //outColor = texture(tex,frag_uv);

    //outColor = texture(tex,vec2(0,0));
    //outColor = vec4(diffuse_light * frag_color + specular_light * frag_color, 1.0) * texture(material,frag_uv);
   //outColor = vec4(diffuse_light * frag_color + specular_light * frag_color, 1.0);
    //outColor = vec4(diffuse_light * vec3(frag_uv,1.0) + specular_light * vec3(frag_uv,1.0), 1.0);
    
}