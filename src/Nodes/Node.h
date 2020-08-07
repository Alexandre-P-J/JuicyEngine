#pragma once
#include <memory.h>
#include <spdlog/spdlog.h>
#include <algorithm>
#include <functional>
#include <list>
#include <queue>

namespace JuicyEngine {
class Node : public std::enable_shared_from_this<Node> {
    /*
        parent may be expired.
        if the parent points nullptr then the node is a root.
        childrens are valid.
        the implicit node tree is cycle free.
    */
    std::weak_ptr<Node> parent;
    std::list<std::shared_ptr<Node>> children;

    Node() = default;

public:
    Node(std::shared_ptr<Node> parent) {
        if (!parent) spdlog::warn("Node created with an invalid parent.");
        this->parent = parent;
        parent->children.push_back(shared_from_this());
    }

    bool reparent(std::shared_ptr<Node> parent) {
        // cycle check
        for (auto next = parent; next; next = next->parent.lock()) {
            if (next.get() == this) {
                spdlog::warn("Node attachment failed. Cycle prevented.");
                return false;
            }
        }
        // dettach from parent
        if (auto old_parent = this->parent.lock()) {
            auto& childs = old_parent->children;
            auto it = std::find_if(childs.begin(), childs.end(),
                                   [this](auto p) { return p.get() == this; });
            if (it != childs.end()) childs.erase(it);
        }
        // attach new parent
        this->parent = parent;
        parent->children.push_back(shared_from_this());
        return true;
    }

    template <typename functype>
    void traverse_up(std::shared_ptr<Node> start,
                     std::function<functype> func) {
        if (!start) return;
        for (auto next = start->parent.lock(); next; next = next->parent.lock())
            func(next);
    }

    template <typename functype>
    void traverse_down(std::shared_ptr<Node> root,
                       std::function<functype> func) {
        if (!root) return;
        std::queue<std::shared_ptr<Node>> q;
        q.push(root);
        while (!q.empty()) {
            auto node = q.front();
            func(node);
            auto& childs = node->children;
            for (auto& child : childs) {
                q.push(child);
            }
            q.pop();
        }
    }
};
}  // namespace JuicyEngine
