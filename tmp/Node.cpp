#include <Node.h>
#include <algorithm>
#include <spdlog/spdlog.h>



Node::Node() {}



Node::Node(std::weak_ptr<Node> parent) { set_parent(parent); }



void Node::set_global_position(glm::vec2 position) {
    glm::vec2 diff = position - get_global_position();
    transform = glm::translate(transform, glm::vec3(diff, 0));
    for (auto& child : children) {
        child->translate(diff);
    }
}



void Node::set_local_position(glm::vec2 position) {
    glm::vec2 diff = position - get_local_position();
    transform = glm::translate(transform, glm::vec3(diff, 0));
    for (auto& child : children) {
        child->translate(diff);
    }
}



glm::vec2 Node::to_global(glm::vec2 vector) {
    return glm::vec2(transform * glm::vec4(vector, 0, 1));
}



glm::vec2 Node::to_local(glm::vec2 vector) {
    auto inv = glm::inverse(transform);
    return glm::vec2(inv * glm::vec4(vector, 0, 1));
}



void Node::translate(const glm::vec2 vector) {
    transform = glm::translate(transform, glm::vec3(vector, 0));
    for (auto& child : children) {
        child->translate(vector);
    }
}



void Node::set_z_index(int z_index) { this->z_index = z_index; }



int Node::get_z_index() const { return z_index; }



void Node::detach_child(std::weak_ptr<Node> node) {
    if (auto sp = parent.lock()) {
        std::remove_if(children.begin(), children.end(),
                       [sp](auto& child) { return child == sp; });
    }
}



void Node::set_parent(std::weak_ptr<Node> parent) {
    if (auto sp = parent.lock()) {
        sp->children.push_back(std::shared_ptr<Node>(this));
        transform = sp->transform;
        this->parent = parent;
        z_index = sp->z_index + 1;
    } else {
        spdlog::warn("Node couldn't be atached to the given parent!");
    }
}



void Node::propagate_update() {
    update();
    for (auto& child : children) {
        child->propagate_update();
    }
}
