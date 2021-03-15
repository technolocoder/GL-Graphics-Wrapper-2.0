#ifndef __SHADER_WRAPPER__
#define __SHADER_WRAPPER__

#include <GL/glew.h>
#include <iostream>

class shader{
public:
    int compile_status;

    shader() {}
    shader(const char *filepath, GLenum _shader_type);
    void initialize(const char *filepath, GLenum _shader_type);

    GLuint operator()();
    GLenum get_type();

    void destroy();
private:
    GLuint shader_id;
    GLenum shader_type;

    static std::string shader_type_str[2];
};

#endif 