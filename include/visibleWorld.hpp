#pragma once

#include "chunk.hpp"

#include "engineDevice.hpp"

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
        visibleWorld(engineDevice& device, int playerX = 0, int playerY = 0);
        ~visibleWorld() = default;

        visibleWorld(const visibleWorld&) = delete;
        void operator=(const visibleWorld&) = delete;

        void updateWorldMesh(int playerX, int playerY, int renderDistance = 3);
        std::vector<std::shared_ptr<chunk>> getNeighbor(const std::pair<int, int> key);
        std::vector<std::shared_ptr<chunk>> getChunkToRender() { return chunkToRender; }

    private:
        std::unordered_map<std::pair<int, int>, std::shared_ptr<chunk>> map;
        std::vector<std::pair<int, int>> chunkLoaded;
        std::vector<std::shared_ptr<chunk>> chunkToRender;

        int chunkX, chunkZ;
        engineDevice& device;

        void clearUnuseChunk(std::vector<std::pair<int, int>> chunkInUse);

    };
}