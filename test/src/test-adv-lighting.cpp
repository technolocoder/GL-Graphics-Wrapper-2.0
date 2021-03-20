#include "gl-wrapper.hpp"
using namespace std;
int main(){
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,2);

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

    program _program("msc/shaders/test-adv-lighting/vs.glsl","msc/shaders/test-adv-lighting/fs.glsl");
    GLuint ubo_projection,ubo_view,ubo_light,ubo_camera;

    glGenBuffers(1,&ubo_projection);
    glBindBuffer(GL_UNIFORM_BUFFER,ubo_projection);
    glBufferData(GL_UNIFORM_BUFFER,sizeof(glm::mat4),glm::value_ptr(glm::perspective(glm::radians(45.0f),(float)window_width/window_height,0.1f,100.0f)),GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER,0,ubo_projection);

    camera _camera({0,0,3});

    glGenBuffers(1,&ubo_view);
    glBindBuffer(GL_UNIFORM_BUFFER,ubo_view);
    glBufferData(GL_UNIFORM_BUFFER,sizeof(glm::mat4),glm::value_ptr(_camera.view_matrix),GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER,1,ubo_view);

    float ambient = 0.01, diffuse = 0.9, specular = 0.5, constant = 1.0, linear = 0.01, quadratic = 0.01;
    glm::vec3 position(0.0,0.0,3.0);
    glm::vec4 a(position,constant),b(glm::vec3(ambient),linear),c(glm::vec3(diffuse),quadratic),d(glm::vec3(specular),0.0);
    glm::mat4 light(a,b,c,d);

    glGenBuffers(1,&ubo_light);
    glBindBuffer(GL_UNIFORM_BUFFER,ubo_light);
    glBufferData(GL_UNIFORM_BUFFER,sizeof(glm::mat4),glm::value_ptr(light),GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER,2,ubo_light);

    glGenBuffers(1,&ubo_camera);
    glBindBuffer(GL_UNIFORM_BUFFER,ubo_camera);
    glBufferData(GL_UNIFORM_BUFFER,sizeof(glm::vec4),glm::value_ptr(glm::vec4(_camera.position,0.0)),GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER,3,ubo_camera);

    _program.use();
    _program.set_mat4x4("model",glm::mat4(1.0));
    _program.set_int("tex",0);

    stbi_set_flip_vertically_on_load(true);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    model _model("msc/models/backpack/backpack.obj",true);

    program fbo_program("msc/shaders/test-adv-lighting/fb_vs.glsl","msc/shaders/test-adv-lighting/fb_fs.glsl");
    fbo_program.use();
    fbo_program.set_int("tex",0);

    float vertices[] = {
        -1,-1,  0,0,
        -1, 1,  0,1,
         1, 1,  1,1,
         1,-1,  1,0,
    };

    unsigned int indices[] = {
        0,1,2,
        2,3,0
    };

    GLuint fbo_vbo, fbo_vao, fbo_ebo;
    glGenBuffers(1,&fbo_vbo);
    glBindBuffer(GL_ARRAY_BUFFER,fbo_vbo);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);

    glGenBuffers(1,&fbo_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,fbo_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);

    glGenVertexArrays(1,&fbo_vao);
    glBindVertexArray(fbo_vao);

    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(float)*4,(void*)0);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(float)*4,(void*)(sizeof(float)*2));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    GLuint fbo_texture,fbo,rbo;
    glGenTextures(1,&fbo_texture);
    glBindTexture(GL_TEXTURE_2D,fbo_texture);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,window_width,window_height,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);

    glGenFramebuffers(1,&fbo);
    glBindFramebuffer(GL_FRAMEBUFFER,fbo);

    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,fbo_texture,0);

    glGenRenderbuffers(1,&rbo);
    glBindRenderbuffer(GL_RENDERBUFFER,rbo);
    glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT32,window_width,window_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,rbo);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        cerr << "Error setting up framebuffer\n";
    }

    glBindRenderbuffer(GL_RENDERBUFFER,0);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    
    SDL_ShowCursor(0);
    glEnable(GL_POINT_SIZE);

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
                glBindBuffer(GL_UNIFORM_BUFFER,ubo_view);
                glBufferSubData(GL_UNIFORM_BUFFER,0,sizeof(glm::mat4),glm::value_ptr(_camera.view_matrix));

                SDL_WarpMouseInWindow(window,window_width/2,window_height/2);
            }
        }
        if(SDL_GetTicks() - reference_tick > frame_delay){
            const unsigned char * keystate = SDL_GetKeyboardState(NULL);
            if(keystate[SDL_SCANCODE_W]){
                _camera.position += _camera.front / glm::vec3(fps) * glm::vec3(2.0);
                _camera.compute_matrices_move();
                glBindBuffer(GL_UNIFORM_BUFFER,ubo_view);
                glBufferSubData(GL_UNIFORM_BUFFER,0,sizeof(glm::mat4),glm::value_ptr(_camera.view_matrix));

                glBindBuffer(GL_UNIFORM_BUFFER,ubo_camera);
                glBufferSubData(GL_UNIFORM_BUFFER,0,sizeof(glm::vec4),glm::value_ptr(glm::vec4(_camera.position,0.0)));
            }
            if(keystate[SDL_SCANCODE_A]){
                _camera.position -= _camera.side / glm::vec3(fps) * glm::vec3(2.0);
                _camera.compute_matrices_move();
                glBindBuffer(GL_UNIFORM_BUFFER,ubo_view);
                glBufferSubData(GL_UNIFORM_BUFFER,0,sizeof(glm::mat4),glm::value_ptr(_camera.view_matrix));

                glBindBuffer(GL_UNIFORM_BUFFER,ubo_camera);
                glBufferSubData(GL_UNIFORM_BUFFER,0,sizeof(glm::vec4),glm::value_ptr(glm::vec4(_camera.position,0.0)));
            }
            if(keystate[SDL_SCANCODE_S]){
                _camera.position -= _camera.front / glm::vec3(fps) * glm::vec3(2.0);
                _camera.compute_matrices_move();
                glBindBuffer(GL_UNIFORM_BUFFER,ubo_view);
                glBufferSubData(GL_UNIFORM_BUFFER,0,sizeof(glm::mat4),glm::value_ptr(_camera.view_matrix));

                glBindBuffer(GL_UNIFORM_BUFFER,ubo_camera);
                glBufferSubData(GL_UNIFORM_BUFFER,0,sizeof(glm::vec4),glm::value_ptr(glm::vec4(_camera.position,0.0)));
            }
            if(keystate[SDL_SCANCODE_D]){
                _camera.position += _camera.side / glm::vec3(fps) * glm::vec3(2.0);
                _camera.compute_matrices_move();
                glBindBuffer(GL_UNIFORM_BUFFER,ubo_view);
                glBufferSubData(GL_UNIFORM_BUFFER,0,sizeof(glm::mat4),glm::value_ptr(_camera.view_matrix));

                glBindBuffer(GL_UNIFORM_BUFFER,ubo_camera);
                glBufferSubData(GL_UNIFORM_BUFFER,0,sizeof(glm::vec4),glm::value_ptr(glm::vec4(_camera.position,0.0)));
            }
            
            glBindFramebuffer(GL_FRAMEBUFFER,fbo);
            glEnable(GL_DEPTH_TEST);

            glClearColor(0.2f,0.2f,0.2f,1.0f);
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
            
            _program.use();
            _model.draw(_program);

            glBindFramebuffer(GL_FRAMEBUFFER,0);

            glDisable(GL_DEPTH_TEST);
            glClear(GL_COLOR_BUFFER_BIT);

            glBindVertexArray(fbo_vao);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,fbo_ebo);
            fbo_program.use();

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D,fbo_texture);

            glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,(void*)0);

            SDL_GL_SwapWindow(window);
            
            reference_tick = SDL_GetTicks();
        }
    }
    
    _model.destroy();
    _program.destroy();
    fbo_program.destroy();
    SDL_DestroyWindow(window);
    SDL_GL_DeleteContext(context);
    SDL_Quit();
    return 0;
}