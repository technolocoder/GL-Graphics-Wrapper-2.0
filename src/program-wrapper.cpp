#include "program-wrapper.hpp"

using namespace std;
program::program() {}

void program::initialize(const char *vs_path, const char *fs_path){
    vs.initialize(vs_path,GL_VERTEX_SHADER); fs.initialize(fs_path,GL_FRAGMENT_SHADER);
    if(vs.compile_status == GL_FALSE || fs.compile_status == GL_FALSE){
        cerr << "Shaders failed compiling\n";
        return;
    }
    program_id = glCreateProgram();
    glAttachShader(program_id,vs());
    glAttachShader(program_id,fs());
    glLinkProgram(program_id);

    glGetProgramiv(program_id,GL_LINK_STATUS,&link_status);
    if(link_status == GL_FALSE){
        int log_length;
        glGetProgramiv(program_id,GL_INFO_LOG_LENGTH,&log_length);
        char log[log_length];
        glGetProgramInfoLog(program_id,log_length,NULL,log);
        cerr << "Error linking program: " << log << '\n';
    }
}

void program::use(){
    glUseProgram(program_id);
}

program::program(const char *vs_path, const char *fs_path){
    initialize(vs_path,fs_path);
}

GLuint program::operator()(){
    return program_id;
}

GLint program::get_uniform_location(const char *uniform_name){
    if(uniform_map.find(uniform_name) != uniform_map.end()) return uniform_map[uniform_name];
    uniform_map[uniform_name] = glGetUniformLocation(program_id,uniform_name);
    return uniform_map[uniform_name];
}


void program::set_int(const char *uniform_name,const int var){
    glUniform1i(get_uniform_location(uniform_name),var);
}

void program::set_float(const char *uniform_name,const float var){
    glUniform1f(get_uniform_location(uniform_name),var);
}

void program::set_double(const char *uniform_name,const double var){
    glUniform1d(get_uniform_location(uniform_name),var);
}

void program::set_vec4f(const char *uniform_name,const glm::vec4 &var){
    glUniform4fv(get_uniform_location(uniform_name),1,glm::value_ptr(var));
}

void program::set_mat4x4(const char *uniform_name, const glm::mat4 &var){
    glUniformMatrix4fv(get_uniform_location(uniform_name),1,GL_FALSE,glm::value_ptr(var));
}

void program::destroy(){
    glDetachShader(program_id,vs());
    glDetachShader(program_id,fs());

    vs.destroy();
    fs.destroy();

    glDeleteProgram(program_id);
}