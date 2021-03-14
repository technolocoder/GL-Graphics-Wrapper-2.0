#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL/SOIL.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
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

struct camera{
    glm::vec3 position,front,side,up = {0.0,1.0,0.0};
    float pitch,yaw;
};

int main(){
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,0);

    const int window_width = 500, window_height = 500;
    SDL_Window *window = SDL_CreateWindow("Test Texture",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,window_width,window_height,SDL_WINDOW_OPENGL);

    SDL_ShowCursor(0);

    SDL_GLContext context = SDL_GL_CreateContext(window);
    glewExperimental = GL_TRUE;
    glewInit();

    GLuint vbo,vao,ebo,texture_id;

    float vertices[] = {
        -0.5,-0.5,0.0, 0,0,
        -0.5, 0.5,0.0, 0,1,
         0.5, 0.5,0.0, 1,1,
         0.5,-0.5,0.0, 1,0
    };

    unsigned int indices[] = {
        0,1,2,
        2,3,0
    };

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

    glGenTextures(1,&texture_id);
    glBindTexture(GL_TEXTURE_2D,texture_id);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

    int texture_width ,texture_height,channels;
    unsigned char *texture = SOIL_load_image("msc/textures/test-texture.jpg",&texture_width,&texture_height,&channels,SOIL_LOAD_AUTO);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB32F,texture_width,texture_height,0,GL_RGB,GL_UNSIGNED_BYTE,texture);
    glGenerateMipmap(GL_TEXTURE_2D);
    free(texture);

    GLuint vs,fs,program;
    vs = create_shader("msc/shaders/texture-vs.glsl",GL_VERTEX_SHADER);
    fs = create_shader("msc/shaders/texture-fs.glsl",GL_FRAGMENT_SHADER);
    program = glCreateProgram();
    glAttachShader(program,vs);
    glAttachShader(program,fs);
    glLinkProgram(program);
    check_program(program);

    glUseProgram(program);
    glUniform1i(glGetUniformLocation(program,"tex"),0);
    glm::mat4 model_matrix = glm::mat4(1.0);
    glUniformMatrix4fv(glGetUniformLocation(program,"model"),1,GL_FALSE,glm::value_ptr(model_matrix));
    glUniformMatrix4fv(glGetUniformLocation(program,"projection"),1,GL_FALSE,glm::value_ptr(glm::perspective(glm::radians(45.0f),(float)window_width/window_height,0.1f,100.0f))); 

    camera cam;
    cam.position = glm::vec3(0.0,0.0,2.0);
    cam.pitch = 0.0;
    cam.yaw = -90;
    cam.front.x = glm::cos(glm::radians(cam.yaw)) * glm::cos(glm::radians(cam.pitch));
    cam.front.y = glm::sin(glm::radians((cam.pitch))); 
    cam.front.z = glm::sin(glm::radians(cam.yaw)) * glm::cos(glm::radians(cam.pitch));
    glm::normalize(cam.front);
    cam.side = glm::cross(cam.front,cam.up);

    glUniformMatrix4fv(glGetUniformLocation(program,"view"),1,GL_FALSE,glm::value_ptr(glm::lookAt(cam.position,cam.position+cam.front,cam.up)));

    SDL_Event event;
    bool quit = false;

    unsigned int fps = 60;
    unsigned int frame_delay = 1000/fps ,reference_tick=0;
    
    glEnable(GL_DEPTH_TEST);
    SDL_WarpMouseInWindow(window,window_width/2,window_height/2); 
    const float sensetivity = 0.1;
    while(!quit){
        const unsigned char *key_state = SDL_GetKeyboardState(NULL);
        if(key_state[SDL_SCANCODE_W]){
            cam.position += cam.front * glm::vec3(0.00002);
            glUniformMatrix4fv(glGetUniformLocation(program,"view"),1,GL_FALSE,glm::value_ptr(glm::lookAt(cam.position,cam.position+cam.front,cam.up)));
        }
        if(key_state[SDL_SCANCODE_A]){
            cam.position -= cam.side * glm::vec3(0.00002);
            glUniformMatrix4fv(glGetUniformLocation(program,"view"),1,GL_FALSE,glm::value_ptr(glm::lookAt(cam.position,cam.position+cam.front,cam.up)));
        }
        if(key_state[SDL_SCANCODE_S]){
            cam.position -= cam.front * glm::vec3(0.00002);
            glUniformMatrix4fv(glGetUniformLocation(program,"view"),1,GL_FALSE,glm::value_ptr(glm::lookAt(cam.position,cam.position+cam.front,cam.up)));
        }
        if(key_state[SDL_SCANCODE_D]){
            cam.position += cam.side * glm::vec3(0.00002);
            glUniformMatrix4fv(glGetUniformLocation(program,"view"),1,GL_FALSE,glm::value_ptr(glm::lookAt(cam.position,cam.position+cam.front,cam.up)));
        }
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
                cam.yaw -= (window_width/2-event.motion.x)*sensetivity;
                cam.pitch += (window_height/2-event.motion.y)*sensetivity;
                //cam.pitch = cam.pitch>89?cam.pitch=89:cam.pitch<-89?cam.pitch=-89:cam.pitch;
                
                cam.front.x = glm::cos(glm::radians(cam.yaw)) * glm::cos(glm::radians(cam.pitch));
                cam.front.y = glm::sin(glm::radians(cam.pitch)); 
                cam.front.z = glm::sin(glm::radians(cam.yaw)) * glm::cos(glm::radians(cam.pitch));
                glm::normalize(cam.front);
                cam.side = glm::cross(cam.front,cam.up);

                glUniformMatrix4fv(glGetUniformLocation(program,"view"),1,GL_FALSE,glm::value_ptr(glm::lookAt(cam.position,cam.position+cam.front,cam.up)));
                SDL_WarpMouseInWindow(window,window_width/2,window_height/2);
            }
        }

        if(SDL_GetTicks()-reference_tick > frame_delay){
            reference_tick = SDL_GetTicks();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glUseProgram(program);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);
            glBindVertexArray(vao);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D,texture_id);

            glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,(void*)0);

            SDL_GL_SwapWindow(window);
        }   
    }

    SDL_GL_DeleteContext(context);
    SDL_Quit();
    return 0;
}