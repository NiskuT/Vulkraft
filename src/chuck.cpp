#include "chunk.hpp"

#include <functional>
#include <random>

#include "utils.hpp"

namespace engine
{
    chunk::chunk(engineDevice& device, int x, int y) : device{device}, x(x), y(y)
    {
        transform.scale = glm::vec3(.1f, .1f, .1f);
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

        /*for (int i = 0; i < CHUNK_SIZE; i++)
        {
            for (int j = 0; j < CHUNK_SIZE; j++)
            {
                // 30 % chance of adding a block
                addBlock(x * CHUNK_SIZE + i, y * CHUNK_SIZE + j, 4, BlockType::DIRT);

            }
        }
        for (int i = 0; i < CHUNK_SIZE; i++)
        {
            for (int j = 0; j < CHUNK_SIZE; j++)
            {
                // 30 % chance of adding a block
                addBlock(x * CHUNK_SIZE + i, y * CHUNK_SIZE + j, 5, BlockType::SAND);

            }
        }*/
        
    }

    void chunk::addBlock(int worldX, int worldY, int worldZ, BlockType blockType)
    {
        auto key = hashFunction(worldX, worldY, worldZ);

        blocks[key] = block(worldX, worldY, worldZ, blockType);
        chunkNeedsUpdate = true;
    }

    void chunk::updateBlockFacesVisible(std::vector<std::shared_ptr<chunk>>& chunks)
    {
        for (auto& block : blocks)
        {
            block.second.updateBlockFacesVisible(chunks);
        }
        chunkNeedsUpdate = false;
        unloadChunkFromDevice();
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

    void chunk::updateVertexBuffers(const std::vector<block::Vertex>& vertices)
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

    void chunk::updateIndexBuffers(const std::vector<uint32_t>& indices)
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