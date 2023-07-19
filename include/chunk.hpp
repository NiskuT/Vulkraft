#pragma once

#include "block.hpp"

#include "engineDevice.hpp"
#include "vulkanBuffer.hpp"
#include "gameObject.hpp"

#include <PerlinNoise.hpp>

#include <unordered_map>
#include <vector>
#include <memory>

#define CHUNK_SIZE 16
#define CHUNK_MAX_Y 40
#define CHUNK_MIN_Y 40

namespace engine
{
    class chunk
    {
        public:
            chunk(engineDevice& device, int x, int z, std::shared_ptr<siv::PerlinNoise> perlin);
            ~chunk() = default;

            void addBlock(int worldX, int worldY, int worldZ, BlockType blockType);
            void updateBlockFacesVisible(std::vector<std::shared_ptr<chunk>>& chunks);
            bool isThereABlockAt(int worldX, int worldY, int worldZ);
            bool isThereAWaterBlockAt(int worldX, int worldY, int worldZ);

            void getPosition(int& x, int& z) { x = this->x; z = this->z; };
            int getX() const { return x; };
            int getZ() const { return z; };

            bool operator==(const chunk& other) const
            {
                return x == other.getX() && z == other.getZ();
            }

            void bind(VkCommandBuffer commandBuffer);
            void draw(VkCommandBuffer commandBuffer);

            void updateChunkMesh();
            void unloadChunkFromDevice();

            bool doesChunkNeedUpdate() { return chunkNeedsUpdate; };
            bool isChunkLoaded() { return chunkIsLoaded; };

            TransformComponent transform{};

        private:
            int x, z;
            std::shared_ptr<siv::PerlinNoise> perlin{nullptr};
            std::unordered_map<unsigned long int, block> blocks;

            engineDevice& device;
            std::unique_ptr<vulkanBuffer> vertexBuffer{nullptr};
            uint32_t vertexCount;
            std::unique_ptr<vulkanBuffer> indexBuffer{nullptr};
            uint32_t indexCount;

            bool chunkIsLoaded{false};
            bool chunkNeedsUpdate{true};

            void updateVertexBuffers(const std::vector<block::Vertex>& vertices);
            void updateIndexBuffers(const std::vector<uint32_t>& indices);

            void generateFlatWorld();
            void generateRandomWorld();

            static unsigned long int hashFunction(int x, int y, int z);

            void getChunkMesh(
                std::unordered_map<block::Vertex, uint32_t>& uniqueVertices, 
                std::vector<block::Vertex>& vertices, 
                std::vector<uint32_t>& indices);

            void generateTree(int x, int y, int z);

    };
}