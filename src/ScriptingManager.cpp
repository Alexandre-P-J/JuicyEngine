#include <Components/ScriptedComponent.h>
#include <Components/TransformComponent.h>
#include <Engine.h>
#include <Logging.h>
#include <ScriptingManager.h>
#include <Serialization.h>
#include <Systems/ScriptedSystem.h>
#include <Utils/ECS.h>
#include <map>
#include <nlohmann/json.hpp>
#include <set>
#include <variant>
#include <vector>

namespace JuicyEngine {

ScriptingManager::ScriptingManager() {
    init_std_libs_and_env();
    init_custom_lib();
    init_ecs();
    init_engine_types();
    init_json_types();
    init_custom_types();
}

void ScriptingManager::run_script(std::string const& script,
                                  std::string const& name) noexcept {
    try {
        lua_state.safe_script(script, lua_env);
    } catch (sol::error const& e) {
        Logging::Lua(Level::critical, e.what());
        Logging::Engine(Level::warning, (name.empty() ? "script" : name) +
                                            " aborted execution");
    }
}

ScriptingManager::~ScriptingManager() {
    SystemFactory::unregister_lua_systems();
    ComponentFactory::unregister_lua_components();
}

void ScriptingManager::init_std_libs_and_env() {
    lua_state.open_libraries(sol::lib::base, sol::lib::coroutine,
                             sol::lib::string, sol::lib::table, sol::lib::math,
                             sol::lib::utf8, sol::lib::bit32);
    lua_env = sol::environment(lua_state, sol::create);

    // STANDARD BASE LIBRARY
    sol::function to_string = lua_state["tostring"];  // used internally
    lua_env["print"] = [to_string](sol::variadic_args const& va) {
        std::string msg;
        for (auto const& v : va) {
            msg.append(to_string(v));
            msg.append("\t");
        }
        msg.pop_back();
        Logging::Lua(Level::info, msg);
    };
    // print but log level error, used as default func error handler, hidden
    // from env
    lua_state["error_handler"] = [to_string](sol::variadic_args const& va) {
        std::string msg;
        for (auto const& v : va) {
            msg.append(to_string(v));
            msg.append("\t");
        }
        msg.pop_back();
        std::string d = "lua: error: ";
        std::string::size_type i = msg.find(d);
        if (i != std::string::npos) msg.erase(i, d.length());
        Logging::Lua(Level::error, msg);
    };
    sol::protected_function::set_default_handler(lua_state["error_handler"]);
    lua_env["warn"] = [to_string](sol::variadic_args const& va) {
        std::string msg;
        for (auto const& v : va) {
            msg.append(to_string(v));
            msg.append("\t");
        }
        msg.pop_back();
        Logging::Lua(Level::warning, msg);
    };
    lua_env["assert"] = lua_state["assert"];
    lua_env["error"] = [to_string](sol::variadic_args const& va) {
        std::string msg;
        for (auto const& v : va) {
            msg.append(to_string(v));
            msg.append("\t");
        }
        msg.pop_back();
        throw sol::error(msg);
    };
    error_func = lua_env["error"];
    lua_env["collectgarbage"] = lua_state["collectgarbage"];
    lua_env["_G"] = lua_env["_ENV"];
    lua_env["getmetatable"] = lua_state["getmetatable"];
    lua_env["ipairs"] = lua_state["ipairs"];
    lua_env["next"] = lua_state["next"];
    lua_env["pairs"] = lua_state["pairs"];
    lua_env["pcall"] = lua_state["pcall"];
    lua_env["rawequal"] = lua_state["rawequal"];
    lua_env["rawget"] = lua_state["rawget"];
    lua_env["rawlen"] = lua_state["rawlen"];
    lua_env["rawset"] = lua_state["rawset"];
    lua_env["select"] = lua_state["select"];
    lua_env["setmetatable"] = lua_state["setmetatable"];
    lua_env["tonumber"] = lua_state["tonumber"];
    lua_env["tostring"] = lua_state["tostring"];
    lua_env["type"] = lua_state["type"];
    lua_env["_VERSION"] = lua_state["_VERSION"];
    lua_env["xpcall"] = lua_state["xpcall"];

    // STANDARD COROUTINE LIBRARY
    lua_env["coroutine"] = lua_state["coroutine"];

    // STANDARD STRING LIBRARY
    lua_env["string"] = lua_state["string"];
    lua_env["string"]["dump"] = sol::nil;

    // STANDARD TABLE LIBRARY
    lua_env["table"] = lua_state["table"];

    // STANDARD MATH LIBRARY
    lua_env["math"] = lua_state["math"];

    // STANDARD UTF8 LIBRARY
    lua_env["utf8"] = lua_state["utf8"];

    // STANDARD BIT32 LIBRARY
    lua_env["bit32"] = lua_state["bit32"];
}

void ScriptingManager::init_custom_lib() {
    // deepcopy(obj)
    const constexpr char* deepcopy_code =
        "function deepcopy(a,b)if type(a)~='table'then return a end;if b and "
        "b[a]then return b[a]end;local c=b or{}local d={}c[a]=d;for e,f in "
        "pairs(a)do d[deepcopy(e,c)]=deepcopy(f,c)end;return "
        "setmetatable(d,getmetatable(a))end";
    lua_state.script(deepcopy_code, lua_env);
    // is_array(table)
    lua_env["is_array"] = [](sol::lua_table const table) {
        for (const auto e : table) {
            if (!e.first.is<uint32_t>()) return false;
        }
        return true;
    };
    // table.has_cycles(obj)
    const constexpr char* has_cycles_code =
        "function table.has_cycles(a)if type(a)~='table'then return false "
        "end;local "
        "b={a}local c=1;local d={table=true}while c~=0 do local "
        "e=b[c]b[c]=nil;c=c-1;for f,g in pairs(e)do if d[g]then return true "
        "elseif type(g)=='table'then d[g]=true;c=c+1;b[c]=g end end end;return "
        "false end";
    lua_state.script(has_cycles_code, lua_env);
    has_cycles = lua_env["table"]["has_cycles"];
}

void ScriptingManager::init_ecs() {
    sol::protected_function deepcopy = lua_env["deepcopy"];
    // Systems
    sol::usertype<SystemFactory> sys_factory =
        lua_env.new_usertype<SystemFactory>("SystemFactory");
    sys_factory["register"] =
        SystemFactory::register_lua_system<ScriptedSystem>;
    sys_factory["register_override"] =
        SystemFactory::register_lua_system_override_cpp<ScriptedSystem>;

    // Components
    sol::usertype<ComponentFactory> comp_factory =
        lua_env.new_usertype<ComponentFactory>("ComponentFactory");
    comp_factory["register"] = [deepcopy](sol::table const& t,
                                          std::string const& name,
                                          sol::this_environment te) {
        bool success =
            ComponentFactory::register_lua_component<ScriptedComponent>(
                t, name, deepcopy);
        if (!success) return;
        sol::environment& env = te;
        sol::usertype<ScriptedComponent> comp =
            env.new_usertype<ScriptedComponent>(name);
        comp["get_component"] = [name](Registry& r, Entity e) -> sol::object {
            auto ptr = r.get<ScriptedComponent>(e, name);
            if (ptr) return ptr->data;
            return sol::nil;
        };
        comp["delete_component"] = [name](Registry& r, Entity e) {
            r.erase_component<ScriptedComponent>(e, name);
        };
        comp["new_component"] = [name, t, deepcopy](Registry& r, Entity e) {
            sol::table new_table = deepcopy(t);
            return r.set_component<ScriptedComponent>(e, name, new_table).data;
        };
    };

    comp_factory["register_override"] = [deepcopy](sol::table t,
                                                   std::string const& name,
                                                   sol::this_environment te) {
        bool success = ComponentFactory::register_lua_component_override_cpp<
            ScriptedComponent>(t, name, deepcopy);
        if (!success) return;
        sol::environment& env = te;
        sol::usertype<ScriptedComponent> comp =
            env.new_usertype<ScriptedComponent>(name);
        comp["get_component"] = [name](Registry& r, Entity e) -> sol::object {
            auto ptr = r.get<ScriptedComponent>(e, name);
            if (ptr) return ptr->data;
            return sol::nil;
        };
        comp["delete_component"] = [name](Registry& r, Entity e) {
            r.erase_component<ScriptedComponent>(e, name);
        };
        comp["new_component"] = [name, t, deepcopy](Registry& r, Entity e) {
            sol::table new_table = deepcopy(t);
            return r.set_component<ScriptedComponent>(e, name, new_table).data;
        };
    };

    // ECS types
    sol::usertype<Entity> entity = lua_env.new_usertype<Entity>(
        "Entity", sol::no_constructor, sol::meta_function::to_string,
        [](Entity const e) {
            return "Entity " + std::to_string(ECS_uint_t(e));
        });
    sol::usertype<Registry> registry = lua_env.new_usertype<Registry>(
        "Registry", sol::constructors<Registry()>());
    registry.set_function("new_entity", &Registry::new_entity);
    registry.set_function(
        "view", [](Registry& r, sol::variadic_args const& va) {
            std::vector<std::string> const args(va.begin(), va.end());
            return r.view(args);
        });
}

void ScriptingManager::init_engine_types() {
    // Engine
    sol::usertype<Engine> engine =
        lua_env.new_usertype<Engine>("Engine", sol::no_constructor);
    engine["instance"] = []() { return std::ref(Engine::instance()); };
    engine["get_scene_manager"] = &Engine::get_scene_manager;

    // Engine: SceneManager
    sol::usertype<SceneManager> scene_manager =
        lua_env.new_usertype<SceneManager>("SceneManager", sol::no_constructor);
    scene_manager["get_scene"] = [](SceneManager& e) {
        return std::ref(e.get_scene());
    };
}

void ScriptingManager::json_save_from_lua(
    nlohmann::json& json, std::variant<int64_t, std::string> const& key,
    sol::object const obj) {
    auto is_array = [](sol::lua_table const table) {
        for (const auto e : table) {
            if (!e.first.is<uint32_t>()) return false;
        }
        return true;
    };
    nlohmann::json* ptr;
    if (key.index() == 1 && !json.is_array()) {
        auto k = std::get<1>(key);
        ptr = &json[k];
    }
    else if (key.index() == 0 && !json.is_object()) {
        auto index = std::get<0>(key) - 1;
        if (index < 0) return;
        while (json.size() <= index) json.push_back(nullptr);
        ptr = &json[index];
    }
    else
        return;
    auto& result = *ptr;
    switch (obj.get_type()) {
        case sol::type::boolean:
            result = obj.as<bool>();
            return;
        case sol::type::lua_nil:
        case sol::type::none:
            return;
        case sol::type::number:
            if (obj.is<int64_t>())
                result = obj.as<int64_t>();
            else
                result = obj.as<double>();
            return;
        case sol::type::string:
            result = obj.as<std::string>();
            return;
        case sol::type::table: {
            auto const self = obj.as<sol::lua_table>();
            if (has_cycles(self)) {
                error_func("Recursion detected while serializing a table");
                return;
            }
            else if (is_array(self)) {
                uint32_t current_index = 1;
                for (auto const e : self) {
                    auto index = e.first.as<uint32_t>();
                    json_save_from_lua(result, index, e.second);
                }
            }
            else {
                for (auto const e : self) {
                    if (e.first.is<std::string>()) {
                        json_save_from_lua(result, e.first.as<std::string>(),
                                           e.second);
                    }
                    else {
                        Logging::Lua(Level::warning,
                                     "Can't serialize non string to json key");
                    }
                }
            }
            return;
        }
        case sol::type::userdata: {
            auto self = obj.as<sol::table>();
            if (sol::protected_function usertype_save = self["save"];
                usertype_save.valid()) {
                usertype_save(obj, result);
                return;
            }
            error_func(
                "Error serializing usertype to JSON, save(self, Json) "
                "function not implemented");
            return;
        }
        default:
            error_func(
                "Error serializing lua object to Json, its type is not "
                "supported");
    }
}

std::variant<std::string, uint64_t, int64_t, double, bool, nlohmann::json,
             std::nullptr_t>
ScriptingManager::json_load_to_lua(nlohmann::json const& json,
                                   sol::object const key) {
    nlohmann::json result;
    if (key.get_type() == sol::type::string && json.is_object()) {
        auto k = key.as<std::string>();
        if (json.contains(k))
            result = json[k];
        else {
            Logging::Lua(Level::warning, "'" + k + "' key not found");
            return nullptr;
        }
    }
    else if (key.get_type() == sol::type::number && json.is_array()) {
        auto index = key.as<std::size_t>() - 1;
        if (json.size() > index)
            result = json[index];
        else {
            Logging::Lua(Level::warning, "index out of bounds");
            return nullptr;
        }
    }
    else
        return nullptr;
    switch (result.type()) {
        case (nlohmann::json::value_t::boolean):
            return bool(result);
        case (nlohmann::json::value_t::string):
            return std::string(result);
        case (nlohmann::json::value_t::number_float):
            return double(result);
        case (nlohmann::json::value_t::number_unsigned):
            return uint64_t(result);
        case (nlohmann::json::value_t::number_integer):
            return int64_t(result);
        case (nlohmann::json::value_t::object):
        case (nlohmann::json::value_t::array):
            return result;
        default:
            Logging::Lua(Level::warning, "unimplemented Json type");
            return nullptr;
    }
}

void ScriptingManager::init_json_types() {
    auto save = [this](nlohmann::json& json,
                       std::variant<int64_t, std::string> const& key,
                       sol::object const obj) {
        return this->json_save_from_lua(json, key, obj);
    };

    auto load = [this](nlohmann::json const& json, sol::object const key) {
        return this->json_load_to_lua(json, key);
    };

    sol::usertype<nlohmann::json> json = lua_env.new_usertype<nlohmann::json>(
        "Json", sol::no_constructor, sol::meta_function::index, load,
        sol::meta_function::new_index, save);
}

void ScriptingManager::init_custom_types() {
    // TransformComponent
    sol::usertype<TransformComponent> tcomp =
        lua_env.new_usertype<TransformComponent>("TransformComponent");
    tcomp["get_position"] = &TransformComponent::get_position;
    tcomp["set_position"] = &TransformComponent::set_position;
    tcomp["get_component"] = [](Registry& r, Entity e) {
        auto ptr = r.get<TransformComponent>(e, TransformComponent::name);
        return ptr;
    };
    tcomp["delete_component"] = [](Registry& r, Entity e) {
        r.erase_component<TransformComponent>(e, TransformComponent::name);
    };
    tcomp["new_component"] = [](Registry& r, Entity e) {
        return r.set_component<TransformComponent>(e, TransformComponent::name);
    };
    define_usertype_serialization(tcomp);

    // VEC3
    sol::usertype<glm::vec3> vec3 = lua_env.new_usertype<glm::vec3>(
        "vec3", sol::constructors<glm::vec3(float, float, float)>(),
        sol::meta_function::to_string, [](glm::vec3 const v) {
            auto str = [](float f) {
                std::string s = std::to_string(f);
                auto it = s.find_last_not_of('0');
                if (s[it] != '.') ++it;
                s.erase(it, std::string::npos);
                return s;
            };
            return "(" + str(v[0]) + ", " + str(v[1]) + ", " + str(v[2]) + ")";
        });
    define_usertype_serialization(vec3);

    // MAT4
    sol::usertype<glm::mat4> mat4 = lua_env.new_usertype<glm::mat4>(
        "mat4", sol::constructors<glm::mat4(), glm::mat4(float),
                                  glm::mat4(glm::vec4, glm::vec4, glm::vec4,
                                            glm::vec4)>());
    define_usertype_serialization(mat4);
}

}  // namespace JuicyEngine
