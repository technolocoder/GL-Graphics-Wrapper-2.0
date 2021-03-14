#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

void check_shader(GLuint shader){
    int compile_status;
    glGetShaderiv(shader,GL_COMPILE_STATUS,&compile_status);
    if(compile_status == GL_FALSE){
        int log_length;
        glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&log_length);
        char log[log_length];
        glGetShaderInfoLog(shader,log_length,NULL,log);
        cerr << "Error compiling shader: " << log << '\n';
    }
}

void check_program(GLuint program){
    int link_status;
    glGetProgramiv(program,GL_LINK_STATUS,&link_status);
    if(link_status == GL_FALSE){
        int log_length;
        glGetProgramiv(program,GL_INFO_LOG_LENGTH,&log_length);
        char log[log_length];
        glGetProgramInfoLog(program,log_length,NULL,log);
        cerr << "Error linking: " << log << '\n';
    }
}

GLuint create_shader(const char *file_path, GLenum shader_type){
    ifstream file(file_path,ios::in);
    if(!file.is_open()){
        cerr << "Error opening file named: " << file_path << '\n';
        return -1;
    }
    ostringstream oss;
    oss << file.rdbuf();
    string str = oss.str();
    const char *src = str.c_str();
    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader,1,&src,NULL);
    glCompileShader(shader);

    check_shader(shader);
    return shader;
}

int main(){
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        cerr << "Error Initializing SDL: " << SDL_GetError() << '\n';
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,0);

    const int window_width = 500, window_height = 500;
    SDL_Window *window = SDL_CreateWindow("OpenGL Rect Test",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,window_width,window_height,SDL_WINDOW_OPENGL);
    if(!window){
        cerr << "Error Creating Window: " << SDL_GetError() << '\n';
        SDL_Quit();
        return -1;
    }

    SDL_GLContext context = SDL_GL_CreateContext(window);
    glewExperimental = GL_TRUE;

    int glew_error_ret;
    if(glew_error_ret = glewInit() != GLEW_OK){
        cerr << "Error Initializing GLEW: " << glewGetErrorString(glew_error_ret) << '\n';
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    
    glViewport(0,0,window_width,window_height);
    float vertices[] = {
        -1,-1, 1.0, 0.0, 0.0,
        -1, 1, 0.0, 1.0, 0.0,
         1,-1, 0.0, 0.0, 1.0,
         1, 1, 1.0, 0.0, 1.0
    };

    unsigned int indices[] = {
        0,1,2,
        1,2,3
    };

    GLuint vbo,ebo,vao;
    glGenBuffers(1,&vbo);
    glGenBuffers(1,&ebo);

    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);
    
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);

    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);
    
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(float)*5,(void*)0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(float)*5,(void*)(sizeof(float)*2));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    char *vertex_shader_src = "#version 400 core\nlayout(location=0) in vec2 position;\nlayout(location=1) in vec3 in_color;\nout vec3 out_color;\nvoid main(){\ngl_Position = vec4(position,1.0,1.0);\nout_color = in_color;\n}";
    char *fragment_shader_src = "#version 400 core\nin vec3 out_color;\nout vec4 _color;\nvoid main(){\n_color = vec4(out_color,1.0);\n}";

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER), fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vertex_shader,1,&vertex_shader_src,NULL);
    glShaderSource(fragment_shader,1,&fragment_shader_src,NULL);

    glCompileShader(vertex_shader);
    glCompileShader(fragment_shader);

    check_shader(vertex_shader);
    check_shader(fragment_shader);

    GLuint program = glCreateProgram();
    glAttachShader(program,vertex_shader);
    glAttachShader(program,fragment_shader);
    glLinkProgram(program);
    check_program(program);
    glUseProgram(program);

    SDL_Event event;
    bool quit = false;

    float fps = 30.0f;
    float frame_delay = 1000.0f/fps;

    unsigned int current_tick,reference_tick = 0;

    while(!quit){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                quit = true;
                break;
            }
        }

        current_tick = SDL_GetTicks();
        if(current_tick-reference_tick>=frame_delay){
            reference_tick = current_tick;

            glClearColor(0.2f,0.2f,0.2f,1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);
            glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,(const void*)0);

            SDL_GL_SwapWindow(window);
        }
    }

    glDeleteBuffers(1,&vbo);
    glDeleteBuffers(1,&ebo);
    glDeleteVertexArrays(1,&vao);

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}