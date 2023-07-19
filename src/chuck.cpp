#include "chunk.hpp"

#include <random>

#include "utils.hpp"
#include <iostream>
#include <cmath>

#define PERLIN_OCTAVE 6
#define FREQUENCY 0.025f
#define SEA_FREQUENCY 0.005f
#define SEA_LEVEL 0
#define HEIGHT_DIFF 0.2f
#define SLOPE 2.f
#define HF_OFFSET 0.3f

namespace engine
{
    chunk::chunk(engineDevice &device, int x, int z, std::shared_ptr<siv::PerlinNoise> perlin)
        : device{device}, x(x), z(z), perlin{perlin}
    {
        transform.scale = glm::vec3(.1f, .1f, .1f);

        if (perlin == nullptr)
        {
            generateFlatWorld();
        }
        else
        {
            generateRandomWorld();
        }
    }

    void chunk::generateFlatWorld()
    {
        for (int i = 0; i < CHUNK_SIZE; i++)
        {
            for (int j = 0; j < CHUNK_SIZE; j++)
            {
                addBlock(x * CHUNK_SIZE + i, CHUNK_MAX_Y + 5, z * CHUNK_SIZE + j, BlockType::BEDROCK);
                for (int k = CHUNK_MAX_Y + 4; k > CHUNK_MAX_Y; k--)
                {
                    addBlock(x * CHUNK_SIZE + i, k, z * CHUNK_SIZE + j, BlockType::DIRT);
                }
                addBlock(x * CHUNK_SIZE + i, CHUNK_MAX_Y, z * CHUNK_SIZE + j, BlockType::GRASS);
            }
        }
    }

    float slopeModifier(float valX)
    {
        //return (0.6f * (1.f - std::sin(valX/(2.f * M_PI))) + 0.4f) - 1.f;
        //return std::sin(valX * 2.f * M_PI);
        return 0.6f * (std::exp(10*valX) - 1.f);
    }

    void chunk::generateRandomWorld()
    {
        const float b = 0.4f;
        for (int i = 0; i < CHUNK_SIZE; i++)
        {
            for (int j = 0; j < CHUNK_SIZE; j++)
            {
                float BF = perlin->normalizedOctave2D((float)(x * CHUNK_SIZE + i) * SEA_FREQUENCY, (float)(z * CHUNK_SIZE + j) * SEA_FREQUENCY, 1);
                float HF = (1 - b) * perlin->normalizedOctave2D_01((float)(x * CHUNK_SIZE + i) * FREQUENCY, (float)(z * CHUNK_SIZE + j) * FREQUENCY, 1) 
                           + b * perlin->normalizedOctave2D_01((float)(x * CHUNK_SIZE + i) * FREQUENCY * 2, (float)(z * CHUNK_SIZE + j) * FREQUENCY, PERLIN_OCTAVE) - HF_OFFSET;


                if (BF > SEA_LEVEL)
                {
                    float height = (1-HEIGHT_DIFF)*HF + HEIGHT_DIFF * BF;
                        
                    height *= -1 * CHUNK_MAX_Y;
                    for (int k = CHUNK_MAX_Y; k > (int)height; k--)
                    {
                        addBlock(x * CHUNK_SIZE + i, k, z * CHUNK_SIZE + j, BlockType::DIRT);
                    }
                    addBlock(x * CHUNK_SIZE + i, height, z * CHUNK_SIZE + j, BlockType::GRASS);
                } else {
                    float height = ((1-HEIGHT_DIFF)*HF + HEIGHT_DIFF * BF);// * std::pow((BF + 1.f), SLOPE);

                    if (height > 0.f)
                    {
                        height *= std::pow((BF + 1.f), SLOPE);                    
                    }
                    else
                    {
                        height = height = slopeModifier(height);
                    }

                    height *= -1 * CHUNK_MAX_Y;

                    for (int k = CHUNK_MAX_Y; k > (int)height; k--)
                    {
                        if (k < 0)
                        {
                            addBlock(x * CHUNK_SIZE + i, k, z * CHUNK_SIZE + j, BlockType::GRASS);
                        }
                        else if (k >= 0 && k < 3)
                        {
                            addBlock(x * CHUNK_SIZE + i, k, z * CHUNK_SIZE + j, BlockType::SAND);
                        }
                        else
                        {
                            addBlock(x * CHUNK_SIZE + i, k, z * CHUNK_SIZE + j, BlockType::STONE);
                        }
                    }
                    for (int k = (int)height; k >= 0; k--)
                    {
                        addBlock(x * CHUNK_SIZE + i, k, z * CHUNK_SIZE + j, BlockType::WATER);
                    }
                }
            }
        }
    }

    void chunk::generateTree(int x, int y, int z)
    {
        const int TREE_HEIGHT = 5;
        const int TREE_TOP_LEAVES = 2;
        for (int i = 0; i < TREE_HEIGHT; ++i)
        {
            addBlock(x, y - i, z, BlockType::WOOD);
        }

        int leafStart = y - TREE_HEIGHT + TREE_TOP_LEAVES;
        int leafEnd = y - TREE_HEIGHT - 1;

        for (int dy = leafStart; dy > leafEnd; --dy)
        {
            int leafRadius = 2 - (dy - leafStart);

            for (int dx = -leafRadius; dx <= leafRadius; ++dx)
            {
                for (int dz = -leafRadius; dz <= leafRadius; ++dz)
                {
                    if (abs(dx) != leafRadius || abs(dz) != leafRadius)
                    {
                        addBlock(x + dx, dy, z + dz, BlockType::LEAVES);
                    }
                }
            }
        }
    }

    void chunk::addBlock(int worldX, int worldY, int worldZ, BlockType blockType)
    {
        auto key = hashFunction(worldX, worldY, worldZ);

        blocks[key] = block(worldX, worldY, worldZ, blockType);
        chunkNeedsUpdate = true;
    }

    void chunk::updateBlockFacesVisible(std::vector<std::shared_ptr<chunk>> &chunks)
    {
        for (auto &block : blocks)
        {
            block.second.updateBlockFacesVisible(chunks);
        }
        chunkNeedsUpdate = false;
        unloadChunkFromDevice();
    }

    bool chunk::isThereABlockAt(int worldX, int worldY, int worldZ)
    {
        auto key = hashFunction(worldX, worldY, worldZ);
        /*        auto key = hashFunction(worldX, worldY, worldZ);
        if (blocks.find(key) != blocks.end() && blocks[key].getBlockType() != BlockType::WATER)
        {
            return true;
        }
        return false;*/

        return blocks.find(key) != blocks.end();
    }

    unsigned long int mix(int a, int b)
    {
        unsigned int ha = static_cast<unsigned int>(a);
        unsigned int hb = static_cast<unsigned int>(b);
        ha = (ha ^ 61) ^ (ha >> 16);
        ha = ha + (ha << 3);
        ha = ha ^ (ha >> 4);
        ha = ha * 0x27d4eb2d;
        hb = (hb ^ 61) ^ (hb >> 16);
        hb = hb + (hb << 3);
        hb = hb ^ (hb >> 4);
        hb = hb * 0x27d4eb2d;
        unsigned long int result = ha + (hb * 0x0000013b);
        return result;
    }

    unsigned long int chunk::hashFunction(int x, int y, int z)
    {
        int dx = std::abs(x) % 16;
        int dy = std::abs(y) % 16;
        int dz = std::abs(z) % 16;

        size_t seed = 0;
        std::hash<int> hasher;

        seed ^= mix(x, dx) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= mix(y, dy) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= mix(z, dz) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

        return static_cast<unsigned long int>(seed);
    }

    void chunk::getChunkMesh(
        std::unordered_map<block::Vertex, uint32_t> &uniqueVertices,
        std::vector<block::Vertex> &vertices,
        std::vector<uint32_t> &indices)
    {
        for (auto &block : blocks)
        {
            block.second.addVerticesToBuffer(uniqueVertices, vertices, indices);
        }
    }

    void chunk::updateVertexBuffers(const std::vector<block::Vertex> &vertices)
    {
        vertexCount = static_cast<uint32_t>(vertices.size());
        assert(vertexCount >= 3 && "Vertex count must be at least 3");

        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
        uint32_t vertexSize = sizeof(vertices[0]);

        vulkanBuffer stagingBuffer(
            device,
            vertexSize,
            vertexCount,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        stagingBuffer.map();
        stagingBuffer.writeToBuffer((void *)vertices.data());

        if (vertexBuffer != nullptr)
        {
            vertexBuffer.reset();
        }

        vertexBuffer = std::make_unique<vulkanBuffer>(
            device,
            vertexSize,
            vertexCount,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        device.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
    }

    void chunk::updateIndexBuffers(const std::vector<uint32_t> &indices)
    {
        indexCount = static_cast<uint32_t>(indices.size());

        VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
        uint32_t indexSize = sizeof(indices[0]);

        vulkanBuffer stagingBuffer(
            device,
            indexSize,
            indexCount,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, // this buffer will be used just has a source location for a memory transfer
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        stagingBuffer.map();
        stagingBuffer.writeToBuffer((void *)indices.data());

        if (indexBuffer != nullptr)
        {
            indexBuffer.reset();
        }

        indexBuffer = std::make_unique<vulkanBuffer>(
            device,
            indexSize,
            indexCount,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        device.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
    }

    void chunk::draw(VkCommandBuffer commandBuffer)
    {
        vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
    }

    void chunk::bind(VkCommandBuffer commandBuffer)
    {
        VkBuffer buffers[] = {vertexBuffer->getBuffer()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
        vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
    }

    // This function does load the chunk to the device if it is not loaded
    void chunk::updateChunkMesh()
    {
        std::unordered_map<block::Vertex, uint32_t> uniqueVertices{};
        std::vector<block::Vertex> vertices{};
        std::vector<uint32_t> indices{};

        getChunkMesh(uniqueVertices, vertices, indices);

        updateVertexBuffers(vertices);
        updateIndexBuffers(indices);

        chunkIsLoaded = true;
    }

    void chunk::unloadChunkFromDevice()
    {
        vertexBuffer.reset();
        indexBuffer.reset();
        chunkIsLoaded = false;
    }

}