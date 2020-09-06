#pragma once
#include <Utils/SparseSet.h>
#include <algorithm>
#include <any>
#include <array>
#include <functional>
#include <limits>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace JuicyEngine {

/*
 * ECS that allows runtime defined components.
 * a component is identified by a name (string) and a real type
 * Example: a component could be a lua table with a type name, all that defined
 * at runtime.
 * */

using ECS_uint_t = std::size_t;
const constexpr ECS_uint_t ECS_uint_min = 0;
const constexpr ECS_uint_t ECS_uint_null =
    std::numeric_limits<ECS_uint_t>::max();
const constexpr ECS_uint_t ECS_uint_max = ECS_uint_null - 1;

class Registry;

class Entity {
    friend Registry;
    ECS_uint_t v;
    inline Entity(ECS_uint_t i) : v(i) {}
    inline operator ECS_uint_t() const { return v; };
};

class Registry {
    // pool definitions
    using pool = std::pair<SparseSet<ECS_uint_t>, std::any>;
    std::vector<pool> pools;
    SparseSet<ECS_uint_t> used_components;

    std::unordered_map<std::string, ECS_uint_t> dynamic_ids;
    ECS_uint_t next_component_id = ECS_uint_min;
    ECS_uint_t next_entity_id = ECS_uint_min;

public:
    inline ECS_uint_t get_component_id(std::string const &type_name) noexcept {
        auto result = dynamic_ids.emplace(type_name, ECS_uint_null);
        if (result.second) {
            result.first->second = next_component_id++;
        }
        return result.first->second;
    }
    inline ECS_uint_t get_component_id(
        std::string const &type_name) const noexcept {
        auto it = dynamic_ids.find(type_name);
        if (it != dynamic_ids.end()) {
            return it->second;
        }
        return ECS_uint_null;
    }

    inline Entity const new_entity() noexcept {
        return Entity{next_entity_id++};
    }

    template <class Component, class... ComponentInitArgs>
    Component &set_component(Entity entity, std::string const &component,
                             ComponentInitArgs &&... component_args) noexcept {
        auto c_id = get_component_id(component);
        auto component_index = used_components.put(c_id);
        if (component_index >= pools.size()) {
            pools.emplace_back(ECS_uint_t(entity), std::vector<Component>());
        }
        auto &vec = std::any_cast<std::vector<Component> &>(
            pools[component_index].second);
        auto entity_index =
            pools[component_index].first.put(ECS_uint_t(entity));
        if (vec.size() >= entity_index) {
            vec.emplace_back(component_args...);
        }
        else {
            vec[entity_index] = Component(component_args...);
        }
        return vec[entity_index];
    }

    template <class Component>
    void erase_component(Entity entity, std::string const &component) noexcept {
        auto c_id = get_component_id(component);
        auto c_index = used_components.get_internal_index(c_id);
        if (c_index == SparseSet<ECS_uint_t>::invalid_index) {
            return;  // component doesn't even exist
        }
        auto transaction = pools[c_index].first.erase(ECS_uint_t(entity));
        if (transaction.first == SparseSet<ECS_uint_t>::invalid_index) {
            return;  // entity doesn't have this component
        }
        auto &v =
            std::any_cast<std::vector<Component> &>(pools[c_index].second);
        v[transaction.first] = v[transaction.second];
        v.pop_back();
    }

    template <class... str>
    std::vector<Entity> const view(
        str const &... component_names) const noexcept {
        static_assert(sizeof...(component_names) > 0);
        static_assert(
            std::conjunction_v<std::is_convertible<str, std::string>...>);
        std::array<ECS_uint_t, sizeof...(component_names)> c_indices{
            used_components.get_internal_index(
                get_component_id(component_names))...};
        if (auto s = pools.size();
            std::any_of(c_indices.begin(), c_indices.end(),
                        [s](auto i) { return i >= s; })) {
            return std::vector<Entity>();
        }
        // search the component id that references the least used component and
        // place it to the beginning of c_idx
        auto it = std::min_element(c_indices.begin(), c_indices.end(),
                                   [this](auto a, auto b) {
                                       return this->pools[a].first.size() <
                                              this->pools[b].first.size();
                                   });
        std::iter_swap(it, c_indices.begin());
        auto &possible_entities =
            pools[*c_indices.begin()].first.get_internal();
        std::vector<Entity> result;
        result.reserve(possible_entities.size());
        auto begin = c_indices.begin();
        ++begin;
        for (auto entity : possible_entities) {
            if (std::all_of(begin, c_indices.end(), [entity, this](auto i) {
                    return this->pools[i].first.has(entity);
                })) {
                result.push_back(Entity{entity});
            }
        }
        return result;
    }

    void clear() {
        pools.clear();
        used_components.clear();
        dynamic_ids.clear();
        next_component_id = ECS_uint_min;
        next_entity_id = ECS_uint_min;
    }

    void visit(Entity const entity,
               std::function<void(std::string const &)> func) const {
        for (auto const &e : dynamic_ids) {
            auto index = used_components.get_internal_index(e.second);
            if (pools.size() < index &&
                pools[index].first.has(ECS_uint_t(entity))) {
                func(e.first);
            }
        }
    }

    template <class Component>
    Component *get(Entity entity, std::string const &component) noexcept {
        auto c_id =
            used_components.get_internal_index(get_component_id(component));
        if (c_id >= pools.size()) return nullptr;
        auto index = pools[c_id].first.get_internal_index(ECS_uint_t(entity));
        auto &vec = std::any_cast<std::vector<Component> &>(pools[c_id].second);
        if (index >= vec.size()) return nullptr;
        return &vec[index];
    }

    template <class Component>
    const Component *get(Entity entity,
                         std::string const &component) const noexcept {
        auto c_id =
            used_components.get_internal_index(get_component_id(component));
        if (c_id >= pools.size()) return nullptr;
        auto index = pools[c_id].first.get_internal_index(ECS_uint_t(entity));
        auto &vec =
            std::any_cast<std::vector<Component> const &>(pools[c_id].second);
        if (index >= vec.size()) return nullptr;
        return &vec[index];
    }
};

}  // namespace JuicyEngine
