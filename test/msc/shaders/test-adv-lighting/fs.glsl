#version 420 core

in VS_OUT{
    vec3 frag_pos;
    vec3 tangent_frag_pos;
    vec3 tangent_camera_pos;
    vec3 tangent_light_pos;
    vec2 texture_coords;
} fs_in;

struct Material{
    sampler2D diffuse1,specular1,normal1;
};

struct point_light{
    vec4 a,b,c,d;
};

layout(std140,binding=2) uniform light{
    point_light pl;
};


uniform Material material;

out vec4 out_color;
void main(){
    vec3 diffuse_tex = texture(material.diffuse1,fs_in.texture_coords).rgb;
    vec3 specular_tex = texture(material.specular1,fs_in.texture_coords).rgb;
    vec3 normal = texture(material.normal1,fs_in.texture_coords).rgb;
    normal = normalize(normal * 2.0 - 1.0);

    vec3 ambient = diffuse_tex * vec3(pl.b);
    
    vec3 light_dir = normalize(fs_in.tangent_light_pos-fs_in.tangent_frag_pos);
    vec3 camera_dir = normalize(fs_in.tangent_camera_pos-fs_in.tangent_frag_pos);
    vec3 halfway_vec = normalize(light_dir+camera_dir);

    float diff = max(dot(light_dir,normal),0.0);
    vec3 diffuse = diffuse_tex * vec3(pl.c) * diff;

    float spec = pow(max(dot(halfway_vec,normal),0.0),32);
    vec3 specular = specular_tex * vec3(pl.d) * spec;

    vec3 color = ambient+diffuse+specular;

    out_color = vec4(color,1.0);
}