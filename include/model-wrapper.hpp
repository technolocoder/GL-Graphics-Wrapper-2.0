#ifndef __MODEL_WRAPPER__
#define __MODEL_WRAPPER__

#include "mesh-wrapper.hpp"
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <map>
#include <iostream>

class model{
public:
    model();
    model(std::string filepath);
    void initialize(std::string filepath);

    void draw(program _program);
    void destroy();
private:
    void comp_size(aiNode *node);
    void process_node(const aiScene *scene, aiNode *node);
    void process_mesh(const aiScene *scene, aiMesh *mesh);
    void load_material(aiMaterial *material,aiTextureType ai_textype, TEXTURE_TYPE texture_type, int count, texture_2d *textures);
    
    std::string path_str;
    int mesh_count = 0, mesh_index = 0;
    mesh *meshes;
    std::map<std::string,texture_2d> mp;
};

#endif