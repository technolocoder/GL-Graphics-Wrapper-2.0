#include "gl-wrapper.hpp"

int main(){
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,0);

    SDL_DisplayMode display_mode;
    SDL_GetDesktopDisplayMode(0,&display_mode);
    const int window_width=display_mode.w,window_height=display_mode.h,fps = 60;
    const float frame_delay = 1000.0/fps,sensetivity = 0.02f;

    SDL_Window *window = SDL_CreateWindow("OpenGL Texture",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,window_width,window_height,SDL_WINDOW_OPENGL|SDL_WINDOW_FULLSCREEN);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    
    glewExperimental = GL_TRUE;
    glewInit();

    SDL_Event event;
    bool quit = false;
    unsigned long long reference_tick;

    texture_2d tex("msc/textures/test-texture.jpg");
    program _program("msc/shaders/test-camera/vs.glsl","msc/shaders/test-camera/fs.glsl");

    camera _camera({0,0,3});
    _program.use();
    _program.set_mat4x4("model",glm::mat4(1.0));
    _program.set_mat4x4("view",_camera.view_matrix);
    _program.set_mat4x4("projection",glm::perspective(glm::radians(45.0f),(float)window_width/window_height,0.1f,100.0f));
    _program.set_int("tex",0);

    float vertices[] = {
        -1,-1, 0, 0,0,
        -1, 1, 0, 0,1,
         1, 1, 0, 1,1,
         1,-1, 0, 1,0
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

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(float)*5,(void*)0);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(float)*5,(void*)(sizeof(float)*3));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    SDL_ShowCursor(0);
    glEnable(GL_DEPTH_TEST);
    while(!quit){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                quit = true;
                break;
            }else if(event.type == SDL_KEYDOWN){
                switch(event.key.keysym.sym){
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                }
            }else if(event.type == SDL_MOUSEMOTION){
                float delta_x = window_width/2.0f-event.motion.x;
                float delta_y = window_height/2.0f-event.motion.y;

                _camera.yaw -= delta_x * sensetivity;
                _camera.pitch += delta_y * sensetivity;
                _camera.pitch = _camera.pitch>89?89:_camera.pitch<-89?-89:_camera.pitch;

                _camera.compute_matrices();
                _program.set_mat4x4("view",_camera.view_matrix);

                SDL_WarpMouseInWindow(window,window_width/2,window_height/2);
            }
        }
        if(SDL_GetTicks() - reference_tick > frame_delay){
            const unsigned char * keystate = SDL_GetKeyboardState(NULL);
            if(keystate[SDL_SCANCODE_W]){
                _camera.position += _camera.front / glm::vec3(fps) * glm::vec3(2.0);
                _camera.compute_matrices_move();
                _program.set_mat4x4("view",_camera.view_matrix);
            }
            if(keystate[SDL_SCANCODE_A]){
                _camera.position -= _camera.side / glm::vec3(fps) * glm::vec3(2.0);
                _camera.compute_matrices_move();
                _program.set_mat4x4("view",_camera.view_matrix);
            }
            if(keystate[SDL_SCANCODE_S]){
                _camera.position -= _camera.front / glm::vec3(fps) * glm::vec3(2.0);
                _camera.compute_matrices_move();
                _program.set_mat4x4("view",_camera.view_matrix);
            }
            if(keystate[SDL_SCANCODE_D]){
                _camera.position += _camera.side / glm::vec3(fps) * glm::vec3(2.0);
                _camera.compute_matrices_move();
                _program.set_mat4x4("view",_camera.view_matrix);
            }

            glClearColor(0.2f,0.2f,0.2f,1.0f);
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

            _program.use();

            glBindVertexArray(vao);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);
            tex.bind_texture(0);

            glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,(void*)0);

            SDL_GL_SwapWindow(window);
            
            reference_tick = SDL_GetTicks();
        }

    }
    
    SDL_GL_DeleteContext(context);
    SDL_Quit();
    return 0;
}