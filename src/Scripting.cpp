#include "Scripting.h"
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>
// needed for registering:
#include <glm/glm.hpp>
#include "Node.h"

std::shared_ptr<Scripting> Scripting::get_instance() {
    static std::shared_ptr<Scripting> ptr(new Scripting);
    return ptr;
}



Scripting::Scripting() {
    state.open_libraries(sol::lib::base, sol::lib::math);
    state.set_panic(sol::c_call<decltype(&Scripting::script_panic),
                                &Scripting::script_panic>);
    register_all();
}


void Scripting::run_script(Node* owner, const std::string& script) {
    context_switch(owner);
    auto result = state.script(script);
    if (!result.valid()) {
        sol::error err = result;
        spdlog::warn(err.what());
    }
}



void Scripting::script_panic(sol::optional<std::string> maybe_msg) {
    if (maybe_msg) {
        spdlog::error(maybe_msg.value());
    } else {
        spdlog::error("LUA threw an unknown panic.");
    }
}


void Scripting::register_all() {
    // DEFINE vec2
    {
        auto add_overload = sol::overload(
            [](const glm::vec2& v1, const glm::vec2& v2) -> glm::vec2 {
                return v1 + v2;
            });
        auto sub_overload = sol::overload(
            [](const glm::vec2& v1, const glm::vec2& v2) -> glm::vec2 {
                return v1 - v2;
            });
        auto mult_overloads = sol::overload(
            [](const glm::vec2& v1, const glm::vec2& v2) -> glm::vec2 {
                return v1 * v2;
            },
            [](const glm::vec2& v1, float f) -> glm::vec2 { return v1 * f; },
            [](float f, const glm::vec2& v1) -> glm::vec2 { return f * v1; });
        auto div_overloads = sol::overload(
            [](const glm::vec2& v1, const glm::vec2& v2) -> glm::vec2 {
                return v1 / v2;
            },
            [](const glm::vec2& v1, float f) -> glm::vec2 { return v1 / f; });
        auto to_string = [](glm::vec2 v) {
            return fmt::format("({}, {})", v.x, v.y);
        };
        sol::usertype<glm::vec2> vec2_type = state.new_usertype<glm::vec2>(
            "vec2", sol::constructors<glm::vec2(float, float), glm::vec2()>(),
            sol::meta_function::addition, add_overload,
            sol::meta_function::subtraction, sub_overload,
            sol::meta_function::multiplication, mult_overloads,
            sol::meta_function::division, mult_overloads,
            sol::meta_function::to_string, to_string);
        vec2_type["abs"] = glm::abs<float>;
        vec2_type["ceil"] = glm::ceil<2, float, glm::defaultp>;
        vec2_type["floor"] = glm::floor<2, float, glm::defaultp>;
        vec2_type["clamp"] = [](glm::vec2 v, float min, float max) {
            return glm::clamp(v, min, max);
        };
        vec2_type["length"] = glm::length<float>;
        vec2_type["angle"] = [](const glm::vec2& v) {
            return glm::atan(v.x, v.y);
        };
        vec2_type["angle_to"] = [](const glm::vec2& v1, const glm::vec2& v2) {
            return glm::acos(glm::dot(v1, v2) /
                             (glm::length(v1) * glm::length(v2)));
        };
        vec2_type["reflect"] = glm::reflect<float>;
        vec2_type["normalize"] = glm::normalize<2, float, glm::defaultp>;
        vec2_type["direction_to"] = [](const glm::vec2& v1,
                                       const glm::vec2& v2) {
            return glm::normalize(v2 - v1);
        };
        vec2_type["distance"] = glm::distance<float>;
        vec2_type["dot"] = glm::dot<float>;
    }
    // DEFINE Node
    {
        sol::usertype<Node> node_type = state.new_usertype<Node>(
            "node", sol::constructors<Node(), Node(std::weak_ptr<Node>)>());
        node_type["get_global_position"] = &Node::get_global_position;
        node_type["set_global_position"] = &Node::set_global_position;
    }
}