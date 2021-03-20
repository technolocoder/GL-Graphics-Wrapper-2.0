#version 400 core

in VS_OUT{
    vec2 texture_coords;
} fs_in;

uniform sampler2D tex;
out vec4 color;
void main(){
    vec3 gamma = pow(texture(tex,fs_in.texture_coords).rgb,vec3(1.0/2.2));
    color = vec4(gamma,1.0);
}