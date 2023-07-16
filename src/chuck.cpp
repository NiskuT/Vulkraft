#include "chunk.hpp"

#include <functional>
#include <random>

namespace engine
{
    chunk::chunk(int x, int y) : x(x), y(y)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < CHUNK_SIZE; j++)
            {
                for (int k = 0; k < CHUNK_SIZE; k++)
                {
                    addBlock(x * CHUNK_SIZE + j, y * CHUNK_SIZE + k, i, BlockType::GRASS);
                }
            }
        }

        for (int i = 0; i < CHUNK_SIZE; i++)
        {
            for (int j = 0; j < CHUNK_SIZE; j++)
            {
                // 30 % chance of adding a block
                if (rand() % 100 < 30)
                    addBlock(x * CHUNK_SIZE + i, y * CHUNK_SIZE + j, 4, BlockType::GRASS);

            }
        }
        
    }

    void chunk::addBlock(int worldX, int worldY, int worldZ, BlockType blockType)
    {
        auto key = hashFunction(worldX, worldY, worldZ);

        blocks[key] = block(worldX, worldY, worldZ, blockType);
    }

    void chunk::updateBlockFacesVisible(std::vector<std::shared_ptr<chunk>>& chunks)
    {
        for (auto& block : blocks)
        {
            block.second.updateBlockFacesVisible(chunks);
        }
    }

    bool chunk::isThereABlockAt(int worldX, int worldY, int worldZ)
    {
        auto key = hashFunction(worldX, worldY, worldZ);

        return blocks.find(key) != blocks.end();
    }

    unsigned long int chunk::hashFunction(int x, int y, int z) {
        size_t seed = 0;
        std::hash<int> hasher;

        seed ^= hasher(x) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= hasher(y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= hasher(z) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

        return static_cast<unsigned long int>(seed);
    }

    void chunk::getChunkMesh(
        std::unordered_map<block::Vertex, uint32_t>& uniqueVertices, 
        std::vector<block::Vertex>& vertices, 
        std::vector<uint32_t>& indices)
    {
        for (auto& block : blocks)
        {
            block.second.addVerticesToBuffer(uniqueVertices, vertices, indices);
        }
    }
}