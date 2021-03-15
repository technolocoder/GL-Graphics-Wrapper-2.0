#ifndef __TEXTURE_WRAPPER__
#define __TEXTURE_WRAPPER__

#include <GL/glew.h>

class texture_2d{
public:
    texture_2d();
    texture_2d(const char *filepath);
    texture_2d(const char *filepath,void(*param_func)());

    void initialize(const char *filepath);
    void initialize(const char *filepath,void(*param_func)());

    void bind_texture();
    void bind_texture(int texture_index);

    GLuint operator()();

    void destroy();
private:
    GLuint texture_id;
};

#endif