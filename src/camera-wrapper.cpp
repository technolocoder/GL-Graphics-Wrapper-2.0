#include "camera-wrapper.hpp"

camera::camera(const glm::vec3 &_position ,const float _pitch ,const float _yaw):position(_position),yaw(_yaw),pitch(_pitch){
    compute_matrices();
}

void camera::initialize(const glm::vec3 &_position ,const float _pitch ,const float _yaw){
    pitch = _pitch;
    yaw = _yaw;

    compute_matrices();
}


void camera::compute_matrices(){
    front.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    front.y = glm::sin(glm::radians(pitch));
    front.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    glm::normalize(front);

    side = glm::cross(front,up);
    glm::normalize(side);

    view_matrix = glm::lookAt(position,position+front,up);
}

void camera::compute_matrices_move(){
    view_matrix = glm::lookAt(position,position+front,up);
}