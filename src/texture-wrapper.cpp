#include "texture-wrapper.hpp"
#define STB_IMAGE_IMPELEMENTATION
#include <stb_image.h>
#include <cstdlib>
#include <iostream>


using namespace std;
texture_2d::texture_2d(){}

texture_2d::texture_2d(const char *filepath){
    initialize(filepath);
}

texture_2d::texture_2d(const char *filepath,void(*param_func)()){
    initialize(filepath,param_func);
}

void texture_2d::initialize(const char *filepath){
    initialize(filepath,[](){
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    });
}

void texture_2d::initialize(const char *filepath,void(*param_func)()){
    glGenTextures(1,&texture_id);
    glBindTexture(GL_TEXTURE_2D,texture_id);
    
    int tex_width, tex_height, channels;
    unsigned char *pixel_data = stbi_load(filepath,&tex_width,&tex_height,&channels,STBI_rgb);
    if(!pixel_data){
        cerr << "Error reading file: " << filepath << '\n';
        return;
    }
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB+channels-3,tex_width,tex_height,0,GL_RGB+channels-3,GL_UNSIGNED_BYTE,pixel_data);
    glGenerateMipmap(GL_TEXTURE_2D);
    free(pixel_data);
}

void texture_2d::bind_texture(){
    glBindTexture(GL_TEXTURE_2D,texture_id);
}  

void texture_2d::bind_texture(int texture_index){
    glActiveTexture(GL_TEXTURE0+texture_index);
    glBindTexture(GL_TEXTURE_2D,texture_id);
}

GLuint texture_2d::operator()(){
    return texture_id;
}

void texture_2d::destroy(){
    glDeleteTextures(1,&texture_id);
}