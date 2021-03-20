#ifndef __MESH_WRAPPER__
#define __MESH_WRAPPER__

#include <GL/glew.h>
#include "texture-wrapper.hpp"
#include "program-wrapper.hpp"
#include <iostream>

class mesh{
public:
    mesh();

    mesh(float *vertices, unsigned int *indices, texture_2d *_textures ,int vertex_count, int _indices_count, int _texture_count);
    void initialize(float *vertices, unsigned int  *indices, texture_2d *_textures, int vertex_count, int indices_count, int texture_count);

    void set_texture_uniforms(program _program);
    void draw();

    void destroy();
private:
    static std::string prefix[3];
    GLuint vbo, ebo, vao;
    texture_2d *textures;
    int indices_count,texture_count;
};

#endif