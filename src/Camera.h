#pragma once
#include "Node.h"

class Camera : public Node {

public:
    Camera(bool active = false);
    glm::mat4 get_view(int min_z_index, int max_z_index);
    glm::mat4 get_projection(int max_z_index);
};