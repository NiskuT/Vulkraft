#pragma once
 
#include <functional>
#include <utility>
#include <glm/glm.hpp>

#include "block.hpp"

namespace engine {

    template <typename T, typename... Rest>
    void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) 
    {
        seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        (hashCombine(seed, rest), ...);
    };
}

namespace std
{
    template <>
    struct hash<engine::block::Vertex>
    {
        size_t operator()(engine::block::Vertex const& vertex) const
        {
            size_t seed = 0;
            engine::hashCombine(seed, vertex.position, vertex.normal, vertex.uv);
            return seed;
        }
    };

    template <>
    struct hash<glm::vec2>
    {
        size_t operator()(glm::vec2 const& vec) const
        {
            size_t seed = 0;
            engine::hashCombine(seed, vec.x, vec.y);
            return seed;
        }
    };

    template <>
    struct hash<glm::vec3>
    {
        size_t operator()(glm::vec3 const& vec) const
        {
            size_t seed = 0;
            engine::hashCombine(seed, vec.x, vec.y, vec.z);
            return seed;
        }
    };


}

