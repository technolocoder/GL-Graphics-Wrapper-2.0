#ifndef __PROGRAM_WRAPPER__
#define __PROGRAM_WRAPPER__

#include "shader-wrapper.hpp"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>

class program{
public:
    int link_status;

    program();

    program(const char *vs_path ,const char *fs_path);
    void initialize(const char *vs_path ,const char *fs_path);

    void set_int(const char *uniform_name,const int var);
    void set_float(const char *uniform_name ,const float var);
    void set_double(const char *uniform_name ,const double var);

    void set_vec4f(const char *uniform_name,const glm::vec4 &var);
    void set_mat4x4(const char *uniform_name,const glm::mat4 &var);

    GLint get_uniform_location(const char *uniform_name);
    void use();

    GLuint operator()();
    void destroy();
private:
    GLuint program_id;
    shader vs,fs;
    std::map<std::string,GLint> uniform_map; 
};

#endif