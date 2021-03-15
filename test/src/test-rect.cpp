#include "gl-wrapper.hpp"

int main(){
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,0);

    SDL_Window *window = SDL_CreateWindow("OpenGL Texture",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,500,500,SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    
    glewExperimental = GL_TRUE;
    glewInit();

    SDL_Event event;
    bool quit = false;

    int fps = 60;
    int frame_delay = 1000/fps;
    unsigned long long reference_tick;

    program _program("msc/shaders/test-rect/vs.glsl","msc/shaders/test-rect/fs.glsl");

    float vertices[] = {
        -1,-1, 0, 1,0,0,
        -1, 1, 0, 0,1,0,
         1, 1, 0, 0,0,1,
         1,-1, 0, 1,1,0
    };

    unsigned int indices[] = {
        0,1,2,
        2,3,0
    };

    GLuint vbo,ebo,vao;
    glGenBuffers(1,&vbo);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);

    glGenBuffers(1,&ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);

    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(float)*6,(void*)0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(float)*6,(void*)(sizeof(float)*3));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    while(!quit){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                quit = true;
                break;
            }        
        }
        if(SDL_GetTicks() - reference_tick > frame_delay){
            glClearColor(0.2f,0.2f,0.2f,1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            _program.use();
            glBindVertexArray(vao);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);

            glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,(void*)0);

            SDL_GL_SwapWindow(window);
            
            reference_tick = SDL_GetTicks();
        }

    }
    
    SDL_GL_DeleteContext(context);
    SDL_Quit();
    return 0;
}