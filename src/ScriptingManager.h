#pragma once
#include <nlohmann/json.hpp>
#include <sol/sol.hpp>

namespace JuicyEngine {

class ScriptingManager {
    friend class Engine;

    sol::state lua_state;
    sol::environment lua_env;

    // create environment, load standard libs and allow secure or modified
    // functions into the environment
    inline void init_std_libs_and_env();
    // define non standard extras
    inline void init_custom_lib();
    // define ECS types and api
    inline void init_ecs();
    // define Engine types
    inline void init_engine_types();
    // initialize json types
    inline void init_json_types();
    // misc
    inline void init_custom_types();

    // helpers and auxiliar functions

    // automatically define both load and save function for the passed usertype
    template <class T>
    void define_usertype_serialization(sol::usertype<T>& usertype) {
        auto inplace_load = [](sol::object o, nlohmann::json const& json) {
            auto& obj = o.as<T>();
            json.get_to(obj);
            return obj;
        };
        auto assign_load = [](nlohmann::json const& json) {
            T obj;
            json.get_to(obj);
            return obj;
        };
        usertype["load"] = sol::overload(inplace_load, assign_load);
        usertype["save"] = [](T const& obj, nlohmann::json& json) {
            json = obj;
        };
    }

    // assign operator for the json usertype
    void json_save_from_lua(
        nlohmann::json& json, std::variant<int64_t, std::string> const& key,
        sol::object const obj);

    // access operator for the json usertype
    std::variant<std::string, uint64_t, int64_t, double, bool,
                        nlohmann::json, std::nullptr_t>
    json_load_to_lua(nlohmann::json const& json, sol::object const key);

    // api accesible error function
    sol::protected_function error_func;

    // api accesible table.has_cycles(obj) function
    sol::protected_function has_cycles;

    
    ScriptingManager();

public:
    void run_script(std::string const& script, std::string const& name = "") noexcept;

    ~ScriptingManager();
    
};

}  // namespace JuicyEngine
