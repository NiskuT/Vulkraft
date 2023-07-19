#include "visibleWorld.hpp"

#include "block.hpp"
#include "utils.hpp"

#include <random>
#include <stdexcept>

namespace engine
{

    visibleWorld::visibleWorld(engineDevice &device, unsigned char mode, int playerX, int playerZ) : device{device}
    {
        chunkX = playerX / CHUNK_SIZE;
        chunkZ = playerZ / CHUNK_SIZE;

        if (mode == 1) {
            auto seed = std::random_device()();
            perlin = std::make_shared<siv::PerlinNoise>(seed);
        }
        else if (mode != 0 && mode != 1) {
            throw std::runtime_error("ERROR: invalid mode");
        }
    }

    void visibleWorld::updateWorldMesh(int playerX, int playerZ, int renderDistance)
    {
        int newChunkX = playerX / CHUNK_SIZE;
        int newChunkZ = playerZ / CHUNK_SIZE;
        if (newChunkX != chunkX || newChunkZ != chunkZ || map.empty())
        {
            chunkX = newChunkX;
            chunkZ = newChunkZ;
            for (int x = chunkX - renderDistance - 1; x <= chunkX + renderDistance; x++)
            {
                for (int z = chunkZ - renderDistance - 1; z <= chunkZ + renderDistance; z++)
                {
                    auto key = std::make_pair(x, z);
                    if (map.find(key) == map.end())
                    {
                        map.emplace(key, std::make_shared<chunk>(device, x, z, perlin));
                    }
                }
            }
        }

        std::vector<std::pair<int, int>> chunkInUse;
        chunkToRender.clear();
        for (int x = chunkX - renderDistance - 1; x <= chunkX + renderDistance; x++)
        {
            for (int z = chunkZ - renderDistance - 1; z <= chunkZ + renderDistance; z++)
            {
                auto key = std::make_pair(x, z);
                if (map.find(key) == map.end())
                {
                    throw std::runtime_error("ERROR: chunk not found in the list of generated chunks");
                }
                if (map[key]->doesChunkNeedUpdate())
                {
                    auto neighbor = getNeighbor(key);
                    map[key]->updateBlockFacesVisible(neighbor);
                }
                if (!map[key]->isChunkLoaded())
                {
                    map[key]->updateChunkMesh();
                    chunkLoaded.push_back(key);
                }
                chunkInUse.push_back(key);
                chunkToRender.push_back(map[key]);
            }
        }
        clearUnuseChunk(chunkInUse);
    }

    std::vector<std::shared_ptr<chunk>> visibleWorld::getNeighbor(const std::pair<int, int> key)
    {
        std::vector<std::shared_ptr<chunk>> neighbor = {map[key]};

        if (map.find(std::make_pair(key.first + 1, key.second)) != map.end())
            neighbor.push_back(map[std::make_pair(key.first + 1, key.second)]);
        if (map.find(std::make_pair(key.first - 1, key.second)) != map.end())
            neighbor.push_back(map[std::make_pair(key.first - 1, key.second)]);
        if (map.find(std::make_pair(key.first, key.second + 1)) != map.end())
            neighbor.push_back(map[std::make_pair(key.first, key.second + 1)]);
        if (map.find(std::make_pair(key.first, key.second - 1)) != map.end())
            neighbor.push_back(map[std::make_pair(key.first, key.second - 1)]);

        return neighbor;
    }

    void visibleWorld::clearUnuseChunk(std::vector<std::pair<int, int>> chunkInUse)
    {
        for (auto &chunk : chunkLoaded)
        {
            if (std::find(chunkInUse.begin(), chunkInUse.end(), chunk) == chunkInUse.end())
            {
                map[chunk]->unloadChunkFromDevice();
                chunkLoaded.erase(std::remove(chunkLoaded.begin(), chunkLoaded.end(), chunk), chunkLoaded.end());
            }
        }
    }
} // namespace engine
