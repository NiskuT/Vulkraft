#include "visibleWorld.hpp"

#include "block.hpp"
#include "utils.hpp"

namespace engine
{
    void visibleWorld::getWorldMesh(std::vector<block::Vertex>& vertices, std::vector<uint32_t>& indices, int playerX, int playerY, int renderDistance)
    {
        int chunkX = playerX / CHUNK_SIZE;
        int chunkY = playerY / CHUNK_SIZE;

        std::unordered_map<block::Vertex, uint32_t> uniqueVertices{};
        std::vector<std::pair<int, int>> newChunks;

        for (int x = chunkX - renderDistance; x <= chunkX + renderDistance; x++)
        {
            for (int y = chunkY - renderDistance; y <= chunkY + renderDistance; y++)
            {
                auto key = std::make_pair(x, y);
                if (map.find(key) == map.end())
                {
                    map.emplace(key, std::make_shared<chunk>(x, y));
                    newChunks.push_back(key);
                }
            }
        }

        for (auto& key : newChunks)
        {
            std::vector<std::shared_ptr<chunk>> neighbor = {map[key]};

            if (map.find( std::make_pair(key.first+1, key.second)) != map.end()) neighbor.push_back(map[std::make_pair(key.first+1, key.second)]);
            if (map.find( std::make_pair(key.first-1, key.second)) != map.end()) neighbor.push_back(map[std::make_pair(key.first-1, key.second)]);
            if (map.find( std::make_pair(key.first, key.second+1)) != map.end()) neighbor.push_back(map[std::make_pair(key.first, key.second+1)]);
            if (map.find( std::make_pair(key.first, key.second-1)) != map.end()) neighbor.push_back(map[std::make_pair(key.first, key.second-1)]);

            map[key]->updateBlockFacesVisible(neighbor);
        }

        for (int x = chunkX - renderDistance; x <= chunkX + renderDistance; x++)
        {
            for (int y = chunkY - renderDistance; y <= chunkY + renderDistance; y++)
            {
                auto key = std::make_pair(x, y);
                map[key]->getChunkMesh(uniqueVertices, vertices, indices);
            }
        }


    }
} // namespace engine
