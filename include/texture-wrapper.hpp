#ifndef __TEXTURE_WRAPPER__
#define __TEXTURE_WRAPPER__

#include <GL/glew.h>

enum TEXTURE_TYPE {TEXTURE_DIFFUSE=0,TEXTURE_SPECULAR};

class texture_2d{
public:
    TEXTURE_TYPE texture_type;

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