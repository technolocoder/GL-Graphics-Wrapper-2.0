#version 420 core

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 texture_coords;

out VS_OUT{
    vec3 normal;
    vec3 frag_pos;
    vec2 texture_coords;
} vs_out;

layout(std140,binding=0) uniform projection_matrix{
    mat4 projection;
};

layout(std140,binding=1) uniform view_matrix{
    mat4 view;
};

uniform mat4 model;

void main(){
    gl_Position = projection * view * model * vec4(position,1.0);
    vs_out.texture_coords = texture_coords;
    vs_out.frag_pos = vec3(model*vec4(position,1.0));
    vs_out.normal = vec3(transpose(inverse(model))*vec4(normal,0.0));
}