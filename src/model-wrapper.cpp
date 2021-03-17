#include "model-wrapper.hpp"
using namespace std;

model::model() {}

void model::initialize(string filepath){
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(filepath,aiProcess_Triangulate|aiProcess_FlipUVs|aiProcess_OptimizeGraph|aiProcess_OptimizeMeshes|aiProcess_ImproveCacheLocality);

    if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
        cout << importer.GetErrorString() << '\n';
    }
    
    path_str = filepath.substr(0,filepath.find_last_of('/'));
    comp_size(scene->mRootNode);
    meshes = (mesh*)malloc(sizeof(mesh)*mesh_count);
    process_node(scene, scene->mRootNode);
}

model::model(string filepath){
    initialize(filepath);
}

void model::process_node(const aiScene *scene ,aiNode *node){
    for(int i = 0; i < node->mNumMeshes; ++i) process_mesh(scene,scene->mMeshes[node->mMeshes[i]]);
    for(int i = 0; i < node->mNumChildren; ++i) process_node(scene,node->mChildren[i]);   
}

void model::process_mesh(const aiScene *scene ,aiMesh *mesh){
    float *vertices = (float*)malloc(mesh->mNumVertices*8*sizeof(float));
    if(mesh->mTextureCoords[0]){
        for(int i = 0; i < mesh->mNumVertices; ++i){
            vertices[i*8] = mesh->mVertices[i].x;
            vertices[i*8+1] = mesh->mVertices[i].y;
            vertices[i*8+2] = mesh->mVertices[i].z; 

            vertices[i*8+3] = mesh->mNormals[i].x;
            vertices[i*8+4] = mesh->mNormals[i].y;
            vertices[i*8+5] = mesh->mNormals[i].z;

            vertices[i*8+6] = mesh->mTextureCoords[0][i].x;
            vertices[i*8+7] = mesh->mTextureCoords[0][i].y;
        }   
    }else{
        for(int i = 0; i < mesh->mNumVertices; ++i){
            vertices[i*8] = mesh->mVertices[i].x;
            vertices[i*8+1] = mesh->mVertices[i].y;
            vertices[i*8+2] = mesh->mVertices[i].z; 

            vertices[i*8+3] = mesh->mNormals[i].x;
            vertices[i*8+4] = mesh->mNormals[i].y;
            vertices[i*8+5] = mesh->mNormals[i].z;

            vertices[i*8+6] = 0;
            vertices[i*8+7] = 0;
        }   
    }

    unsigned int total_indices = 0, offset = 0;
    for(int i = 0; i < mesh->mNumFaces; ++i) total_indices += mesh->mFaces[i].mNumIndices;
    unsigned int *indices = (unsigned int*)malloc(sizeof(int)*total_indices);
    for(int i = 0; i < mesh->mNumFaces; ++i){
        for(int j = 0; j < mesh->mFaces[i].mNumIndices; ++j) indices[offset+j] = mesh->mFaces[i].mIndices[j];
        offset += mesh->mFaces[i].mNumIndices;
    }
    aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];
    int diffuse_count = mat->GetTextureCount(aiTextureType_DIFFUSE);
    int specular_count = mat->GetTextureCount(aiTextureType_SPECULAR);
    texture_2d textures[diffuse_count+specular_count];
    if(diffuse_count > 0) load_material(mat,aiTextureType_DIFFUSE,TEXTURE_DIFFUSE,diffuse_count,textures);
    if(specular_count > 0) load_material(mat,aiTextureType_SPECULAR,TEXTURE_SPECULAR,specular_count,textures+diffuse_count);    

    meshes[mesh_index++].initialize(vertices,indices,textures,mesh->mNumVertices,total_indices,diffuse_count+specular_count);
    free(indices);
    free(vertices);
}


void model::load_material(aiMaterial *material,aiTextureType ai_textype, TEXTURE_TYPE texture_type, int count, texture_2d *textures){
    for(int i = 0; i < count; ++i){
        aiString path;
        material->GetTexture(ai_textype,i,&path);
        const char *file = path.C_Str();

        string temp = path_str+string(1,'/')+string(file);
        if(mp.find(temp) == mp.end()){
            textures[i].initialize(temp.c_str());
            textures[i].texture_type = texture_type;
            mp[temp] = textures[i];
        }else{
            textures[i] = mp[temp];
        }
    }
}

void model::comp_size(aiNode *node){
    mesh_count += node->mNumMeshes;
    for(int i = 0; i < node->mNumChildren; ++i) comp_size(node->mChildren[i]);
}

void model::draw(program _program){
    for(int i = 0; i < mesh_count; ++i){
        meshes[i].set_texture_uniforms(_program);
        meshes[i].draw();
    }
}

void model::destroy(){
    for(int i = 0; i < mesh_count; ++i) meshes[i].destroy();
    free(meshes);
}