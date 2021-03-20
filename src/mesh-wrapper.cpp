#include "mesh-wrapper.hpp"
#include <cstdlib>
#include <cstring>

using namespace std;
string mesh::prefix[3] = {"material.diffuse","material.specular","material.normal"};

mesh::mesh(){}

void mesh::initialize(float *vertices, unsigned int  *indices, texture_2d *_textures, int vertex_count, int _indices_count, int _texture_count){
    texture_count = _texture_count;
    indices_count = _indices_count;

    textures = (texture_2d*)malloc(sizeof(texture_2d)*texture_count);
    memcpy(textures,_textures,sizeof(texture_2d)*texture_count);

    glGenBuffers(1,&vbo);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER,sizeof(float)*14*vertex_count,vertices,GL_STATIC_DRAW);

    glGenBuffers(1,&ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(int)*indices_count,indices,GL_STATIC_DRAW);

    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(float)*14,(void*)0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(float)*14,(void*)(sizeof(float)*3));
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(float)*14,(void*)(sizeof(float)*6));
    glVertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,sizeof(float)*14,(void*)(sizeof(float)*8));
    glVertexAttribPointer(4,3,GL_FLOAT,GL_FALSE,sizeof(float)*14,(void*)(sizeof(float)*11));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
}

mesh::mesh(float *vertices, unsigned int *indices, texture_2d *_textures ,int vertex_count, int indices_count, int texture_count){
    initialize(vertices,indices,_textures,vertex_count,indices_count,texture_count);
}
    
void mesh::set_texture_uniforms(program _program){
    int count[3];
    memset(count,0,sizeof(int)*3);
    for(int i = 0; i < texture_count; ++i){
        textures[i].bind_texture(i);
        count[textures[i].texture_type]++;
        _program.set_int((prefix[textures[i].texture_type]+to_string(count[textures[i].texture_type])).c_str(),i); 
    }
}

void mesh::draw(){
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);
    
    glDrawElements(GL_TRIANGLES,indices_count,GL_UNSIGNED_INT,(void*)0);
}

void mesh::destroy(){
    for(int i = 0; i < texture_count; ++i) textures[i].destroy();
    free(textures);
    glDeleteBuffers(1,&vbo);
    glDeleteBuffers(1,&ebo);
    glDeleteVertexArrays(1,&vao);
}