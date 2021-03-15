#include "shader-wrapper.hpp"
#include <fstream>
#include <sstream>

std::string shader::shader_type_str[2] = {"FRAGMENT SHADER","VERTEX SHADER"};

using namespace std;
void shader::initialize(const char *filepath, GLenum _shader_type){
    shader_type = _shader_type;
    
    ifstream file(filepath);
    if(!file.is_open()){
        cerr << "Error opening file named: " << filepath << " (" << shader_type_str[shader_type-GL_FRAGMENT_SHADER] << ")\n";
        return;
    }
    
    ostringstream oss;
    oss << file.rdbuf();
    string str = oss.str();
    const char *source_str = str.c_str();

    shader_id = glCreateShader(shader_type);
    glShaderSource(shader_id,1,&source_str,NULL);
    glCompileShader(shader_id);

    glGetShaderiv(shader_id,GL_COMPILE_STATUS,&compile_status);

    if(compile_status == GL_FALSE){
        int log_size;
        glGetShaderiv(shader_id,GL_INFO_LOG_LENGTH,&log_size);
        char log[log_size];
        glGetShaderInfoLog(shader_id,log_size,NULL,log);
        cerr << "Logs Compiling: " << filepath << " (" << shader_type_str[shader_type-GL_FRAGMENT_SHADER] << "):\n" << log << '\n';
    }
}

shader::shader(const char *filepath, GLenum _shader_type){
    initialize(filepath,_shader_type);
}

GLuint shader::operator()(){
    return shader_id;
}

GLenum shader::get_type(){
    return shader_type;
}

void shader::destroy(){
    glDeleteShader(shader_id);
}