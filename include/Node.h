#pragma once
#include <bx/math.h>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vector>

class Node {
protected:
    glm::mat4 transform =
        glm::identity<glm::mat4>(); // transform * local_vector = global_vector
    std::weak_ptr<Node> parent;
    std::vector<std::shared_ptr<Node>> children;
    int z_index = 0;

public:
    Node();
    Node(std::weak_ptr<Node> parent);

    inline glm::vec2 get_global_position() { return glm::vec2(transform[3]); }
    inline glm::vec2 get_local_position() {
        return glm::vec2(glm::inverse(transform)[3]);
    }
    void set_global_position(glm::vec2 position);
    void set_local_position(glm::vec2 position);
    inline glm::vec2 to_global(glm::vec2 vector);
    inline glm::vec2 to_local(glm::vec2 vector);
    void translate(const glm::vec2 vector);
    void scale(const glm::vec2 vector);
    void rotate(const float radians);
    void set_z_index(int z_index);
    int get_z_index() const;
    void detach_child(std::weak_ptr<Node> node);
    void set_parent(std::weak_ptr<Node> parent);
    void propagate_update();
    virtual void update() {}
    virtual void draw() {}
};