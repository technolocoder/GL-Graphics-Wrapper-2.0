#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct camera{
    float pitch,yaw;
    glm::vec3 position,front,side,up = {0,1,0};
    glm::mat4 view_matrix;

    void initialize(const glm::vec3 &_position ,const float _pitch ,const float _yaw);
    void compute_matrices();
};  