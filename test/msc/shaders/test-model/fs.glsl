#version 420 core

in VS_OUT{
    vec2 texture_coords;
} fs_in;

uniform sampler2D diffuse1;
uniform sampler2D specular1;

out vec4 color;
void main(){
    color = texture(diffuse1,fs_in.texture_coords);
}