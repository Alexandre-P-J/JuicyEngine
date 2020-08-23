#pragma once
#include <functional>
#include <memory>
#include <tuple>

namespace JuicyEngine {

template <class ResourceType, class... DependenciesTypes>
class Resource;

template <class ResourceType, class... DependenciesTypes>
class ResourceHandle {
    friend Resource<ResourceType, DependenciesTypes...>;
    std::weak_ptr<ResourceType> data;
    std::tuple<ResourceHandle<DependenciesTypes>...> deps;

public:
    // Default constructible, the resource is invalid
    ResourceHandle() = default;
    // Construct from Resource conversion
    ResourceHandle(Resource<ResourceType, DependenciesTypes...> resource)
        : data(resource.data), deps(resource.deps) {}
    // Check if Resource is alive
    inline operator bool() const { return bool(data.lock()); }
    // Get pointer to Resource, bool() operator should be called before this.
    inline ResourceType const &operator*() { return *(data.lock()); }
};

template <class ResourceType, class... DependenciesTypes>
class Resource {
    friend ResourceHandle<ResourceType, DependenciesTypes...>;
    std::shared_ptr<ResourceType> data;
    std::tuple<Resource<DependenciesTypes>...> deps;

public:
    // Default constructible, the resource is invalid
    Resource() = default;
    // I. unmanaged raw ptr + custom destructor + dependencies
    Resource(ResourceType *resource,
             std::function<void(ResourceType *)> destructor,
             Resource<DependenciesTypes>... dependencies)
        : data(resource, destructor), deps(dependencies...) {}

    // II. unmanaged raw ptr + default destructor + dependencies
    Resource(ResourceType *resource,
             Resource<DependenciesTypes>... dependencies)
        : data(resource), deps(dependencies...) {}

    // III. shared_ptr + dependencies
    Resource(std::shared_ptr<ResourceType> resource,
             Resource<DependenciesTypes>... dependencies)
        : data(resource), deps(dependencies...) {}

    // IV. unmanaged raw ptr + custom destructor + dependencies as tuple
    Resource(ResourceType *resource,
             std::function<void(ResourceType *)> destructor,
             std::tuple<Resource<DependenciesTypes>...> dependencies)
        : data(resource, destructor), deps(dependencies) {}

    // V. unmanaged raw ptr + default destructor + dependencies as tuple
    Resource(ResourceType *resource,
             std::tuple<Resource<DependenciesTypes>...> dependencies)
        : data(resource), deps(dependencies) {}

    // VI. shared_ptr + dependencies as tuple
    Resource(std::shared_ptr<ResourceType> resource,
             std::tuple<Resource<DependenciesTypes>...> dependencies)
        : data(resource), deps(dependencies) {}
    // VII. Construct from resource handle conversion, the resource could be
    // invalid
    Resource(ResourceHandle<ResourceType, DependenciesTypes...> const &handle)
        : data(handle.data.lock()), deps(handle.deps) {}

    // conversion used when passing a Resource with dependencies to constructors
    // IV, V, VI
    operator std::tuple<Resource<ResourceType>,
                        Resource<DependenciesTypes>...>() const {
        return std::tuple_cat(std::make_tuple(Resource<ResourceType>(data)),
                              deps);
    }
    // check if the resource is not null
    inline operator bool() const { return bool(data); }
    // get the resource, bool() operator should be called before this
    inline ResourceType const &operator*() { return *data; }
};

}  // namespace JuicyEngine
