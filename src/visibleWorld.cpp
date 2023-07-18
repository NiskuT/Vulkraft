#include "visibleWorld.hpp"

#include "block.hpp"
#include "utils.hpp"

#include <iostream>

namespace engine
{

    visibleWorld::visibleWorld(engineDevice &device, int playerX, int playerZ) : device{device}
    {
        chunkX = playerX / CHUNK_SIZE;
        chunkZ = playerZ / CHUNK_SIZE;
    }

    void visibleWorld::updateWorldMesh(int playerX, int playerZ, int renderDistance)
    {
        int newChunkX = playerX / CHUNK_SIZE;
        int newChunkZ = playerZ / CHUNK_SIZE;
        if (newChunkX != chunkX || newChunkZ != chunkZ || map.empty())
        {
            chunkX = newChunkX;
            chunkZ = newChunkZ;
            for (int x = chunkX - renderDistance; x <= chunkX + renderDistance; x++)
            {
                for (int z = chunkZ - renderDistance; z <= chunkZ + renderDistance; z++)
                {
                    auto key = std::make_pair(x, z);
                    if (map.find(key) == map.end())
                    {
                        map.emplace(key, std::make_shared<chunk>(device, x, z));
                    }
                }
            }
        }

        std::vector<std::pair<int, int>> chunkInUse;
        chunkToRender.clear();
        for (int x = chunkX - renderDistance; x <= chunkX + renderDistance; x++)
        {
            for (int z = chunkZ - renderDistance; z <= chunkZ + renderDistance; z++)
            {
                auto key = std::make_pair(x, z);
                if (map.find(key) == map.end())
                {
                    std::cout << "ERROR: chunk not found" << std::endl;
                }
                if (map[key]->doesChunkNeedUpdate())
                {
                    auto neighbor = getNeighbor(key);
                    map[key]->updateBlockFacesVisible(neighbor);
                }
                if (!map[key]->isChunkLoaded())
                {
                    map[key]->updateChunkMesh();
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
