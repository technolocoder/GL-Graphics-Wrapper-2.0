#include "model-wrapper.hpp"
using namespace std;

model::model() {}

void model::initialize(string filepath,bool _gamma_corrected){
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(filepath,aiProcess_Triangulate|aiProcess_FlipUVs|aiProcess_OptimizeGraph|aiProcess_OptimizeMeshes|aiProcess_ImproveCacheLocality|aiProcess_CalcTangentSpace);

    if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
        cout << importer.GetErrorString() << '\n';
    }
    
    gamma_corrected = _gamma_corrected;
    path_str = filepath.substr(0,filepath.find_last_of('/'));
    comp_size(scene->mRootNode);
    meshes = (mesh*)malloc(sizeof(mesh)*mesh_count);
    process_node(scene, scene->mRootNode);
}

model::model(string filepath,bool _gamma_corrected){
    initialize(filepath,_gamma_corrected);
}

void model::process_node(const aiScene *scene ,aiNode *node){
    for(int i = 0; i < node->mNumMeshes; ++i) process_mesh(scene,scene->mMeshes[node->mMeshes[i]]);
    for(int i = 0; i < node->mNumChildren; ++i) process_node(scene,node->mChildren[i]);   
}

void model::process_mesh(const aiScene *scene ,aiMesh *mesh){
    float *vertices = (float*)malloc(mesh->mNumVertices*14*sizeof(float));
    if(mesh->mTextureCoords[0]){
        for(int i = 0; i < mesh->mNumVertices; ++i){
            vertices[i*14] = mesh->mVertices[i].x;
            vertices[i*14+1] = mesh->mVertices[i].y;
            vertices[i*14+2] = mesh->mVertices[i].z; 

            vertices[i*14+3] = mesh->mNormals[i].x;
            vertices[i*14+4] = mesh->mNormals[i].y;
            vertices[i*14+5] = mesh->mNormals[i].z;

            vertices[i*14+6] = mesh->mTextureCoords[0][i].x;
            vertices[i*14+7] = mesh->mTextureCoords[0][i].y;

            vertices[i*14+8] = mesh->mTangents[i].x;
            vertices[i*14+9] = mesh->mTangents[i].y;
            vertices[i*14+10] = mesh->mTangents[i].z;

            vertices[i*14+11] = mesh->mBitangents[i].x;
            vertices[i*14+12] = mesh->mBitangents[i].y;
            vertices[i*14+13] = mesh->mBitangents[i].z;
        }   
    }else{
        for(int i = 0; i < mesh->mNumVertices; ++i){
            vertices[i*14] = mesh->mVertices[i].x;
            vertices[i*14+1] = mesh->mVertices[i].y;
            vertices[i*14+2] = mesh->mVertices[i].z; 

            vertices[i*14+3] = mesh->mNormals[i].x;
            vertices[i*14+4] = mesh->mNormals[i].y;
            vertices[i*14+5] = mesh->mNormals[i].z;

            vertices[i*14+6] = 0;
            vertices[i*14+7] = 0;

            vertices[i*14+8] = mesh->mTangents[i].x;
            vertices[i*14+9] = mesh->mTangents[i].y;
            vertices[i*14+10] = mesh->mTangents[i].z;

            vertices[i*14+11] = mesh->mBitangents[i].x;
            vertices[i*14+12] = mesh->mBitangents[i].y;
            vertices[i*14+13] = mesh->mBitangents[i].z;
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
    int normal_count = mat->GetTextureCount(aiTextureType_HEIGHT);

    texture_2d textures[diffuse_count+specular_count+normal_count];
    if(diffuse_count > 0) load_material(mat,aiTextureType_DIFFUSE,TEXTURE_DIFFUSE,diffuse_count,textures,gamma_corrected);
    if(specular_count > 0) load_material(mat,aiTextureType_SPECULAR,TEXTURE_SPECULAR,specular_count,textures+diffuse_count,false);    
    if(normal_count > 0) load_material(mat,aiTextureType_HEIGHT,TEXTURE_NORMAL,normal_count,textures+(diffuse_count+specular_count),false);

    meshes[mesh_index++].initialize(vertices,indices,textures,mesh->mNumVertices,total_indices,diffuse_count+specular_count+normal_count);
    free(indices);
    free(vertices);
}


void model::load_material(aiMaterial *material,aiTextureType ai_textype, TEXTURE_TYPE texture_type, int count, texture_2d *textures,bool _gamma_corrected){
    for(int i = 0; i < count; ++i){
        aiString path;
        material->GetTexture(ai_textype,i,&path);
        const char *file = path.C_Str();

        string temp = path_str+string(1,'/')+string(file);
        if(mp.find(temp) == mp.end()){
            textures[i].initialize(temp.c_str(),_gamma_corrected);
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