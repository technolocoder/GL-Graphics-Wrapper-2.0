#version 420 core

in VS_OUT{
    vec3 normal;
    vec3 frag_pos;
    vec2 texture_coords;
} fs_in;

struct Material{
    sampler2D diffuse1,specular1;
};

struct point_light{
    vec4 a,b,c,d;
};

layout(std140,binding=2) uniform light{
    point_light pl;
};

layout(std140,binding=3) uniform camera{
    vec3 camera_pos;
};

uniform Material material;

out vec4 out_color;
void main(){
    vec3 diffuse_tex = texture(material.diffuse1,fs_in.texture_coords).rgb;
    vec3 specular_tex = texture(material.specular1,fs_in.texture_coords).rgb;

    vec3 ambient = diffuse_tex * vec3(pl.b);
    vec3 light_dir = normalize(vec3(pl.a)-fs_in.frag_pos);
    vec3 norm = normalize(fs_in.normal);

    float diff = max(dot(light_dir,norm),0.0);
    vec3 diffuse = diffuse_tex*diff*vec3(pl.c);

    vec3 camera_dir = normalize(vec3(camera_pos)-fs_in.frag_pos);
    vec3 halfway_dir = normalize(camera_dir+light_dir);

    float spec = pow(max(dot(halfway_dir,norm),0.0),32);
    vec3 specular = specular_tex*spec*vec3(pl.d);

    vec3 color = ambient+diffuse+specular;

    out_color = vec4(color,1.0);
}