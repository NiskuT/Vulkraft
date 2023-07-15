#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include <vector>
#include <unordered_map>

#define TOP_FACE_VISIBLE(blockFacesVisible) ((blockFacesVisible) & 0b000001)
#define BOTTOM_FACE_VISIBLE(blockFacesVisible) ((blockFacesVisible) & 0b000010)
#define NORTH_FACE_VISIBLE(blockFacesVisible) ((blockFacesVisible) & 0b000100)
#define SOUTH_FACE_VISIBLE(blockFacesVisible) ((blockFacesVisible) & 0b001000)
#define EAST_FACE_VISIBLE(blockFacesVisible) ((blockFacesVisible) & 0b010000)
#define WEST_FACE_VISIBLE(blockFacesVisible) ((blockFacesVisible) & 0b100000)

namespace engine
{
    enum class BlockType
    {
        DIRT,
        GRASS,
        STONE,
        WOOD,
        LEAVES,
        WATER,
        SAND
    };

    class block
    {
        public:
            struct Vertex
            {
                glm::vec3 position;
                glm::vec2 uv;

                static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
                static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

                bool operator==(const Vertex& other) const
                {
                    return position == other.position && uv == other.uv;
                }
            };

            block(int x, int y, int z, BlockType blocType) : pos{x, y, z}, blockFacesVisible(0b111111), blockType(blockType) {};
            ~block();

            block(const block&) = delete;
            void operator=(const block&) = delete;

            void updateBlockFacesVisible();
            void addVerticesToBuffer(
                std::unordered_map<Vertex, uint32_t>& uniqueVertices, 
                std::vector<Vertex>& vertices, 
                std::vector<uint32_t>& indices, 
                int chunkX, int chunkY);

            static void pushVertex(
                std::unordered_map<Vertex, uint32_t>& uniqueVertices, 
                std::vector<Vertex>& vertices,
                std::vector<uint32_t>& indices, 
                const block::Vertex& vertex);

        private:
            int pos[3];
            unsigned char blockFacesVisible;
            BlockType blockType; 

    };
}