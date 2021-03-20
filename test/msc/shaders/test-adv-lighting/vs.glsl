#version 420 core

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 texture_coords;
layout(location=3) in vec3 tangent;

struct point_light{
    vec4 a,b,c,d;
};

layout(std140,binding=2) uniform light{
    point_light pl;
};

layout(std140,binding=3) uniform camera{
    vec3 camera_pos;
};

out VS_OUT{
    vec3 frag_pos;
    vec3 tangent_frag_pos;
    vec3 tangent_camera_pos;
    vec3 tangent_light_pos;
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
    mat3 norm_matrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(norm_matrix * tangent);
    vec3 N = normalize(norm_matrix * normal);
    T = normalize(T-dot(T,N)*N);
    vec3 B = cross(N,T);
    mat3 TBN = transpose(mat3(T,B,N));

    vs_out.frag_pos = vec3(model * vec4(position,1.0));
    vs_out.tangent_frag_pos = TBN * vs_out.frag_pos;
    vs_out.tangent_camera_pos = TBN * camera_pos;
    vs_out.tangent_light_pos = TBN * vec3(pl.a);
    vs_out.texture_coords = texture_coords;
    gl_Position = projection * view * model * vec4(position,1.0);
}