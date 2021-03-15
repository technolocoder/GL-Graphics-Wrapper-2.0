#ifndef __CAMERA_WRAPPER__
#define __CAMERA_WRAPPER__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct camera{
    float pitch,yaw;
    glm::vec3 position,front,side,up = {0,1,0};
    glm::mat4 view_matrix;

    camera(const glm::vec3 &_position={0,0,2} ,const float _pitch=0 ,const float _yaw=-90);

    void initialize(const glm::vec3 &_position={0,0,2} ,const float _pitch=0 ,const float _yaw=-90);
    
    void compute_matrices();
    void compute_matrices_move();
};  

#endif 