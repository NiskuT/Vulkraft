#pragma once

#include "chunk.hpp"

#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace std
{
    template <>
    struct hash<std::pair<int, int>> {
        size_t operator()(const std::pair<int, int>& p) const {
            size_t seed = 0;
            seed ^= std::hash<int>{}(p.first) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= std::hash<int>{}(p.second) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            return seed;
        }
    };
} 

namespace engine
{
    class visibleWorld
    {
    public:
        visibleWorld();
        ~visibleWorld();

        visibleWorld(const visibleWorld&) = delete;
        void operator=(const visibleWorld&) = delete;

        void getWorldMesh(std::vector<block::Vertex>& vertices, std::vector<uint32_t>& indices, int playerX, int playerY, int renderDistance = 3);

    private:
        std::unordered_map<std::pair<int, int>, std::shared_ptr<chunk>> map;

    };
}