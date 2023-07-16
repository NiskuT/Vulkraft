#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include <vector>
#include <unordered_map>
#include <memory>

#define TOP_FACE 0b000001
#define BOTTOM_FACE 0b000010
#define NORTH_FACE 0b000100
#define SOUTH_FACE 0b001000
#define EAST_FACE 0b010000
#define WEST_FACE 0b100000

#define TOP_FACE_VISIBLE(blockFacesVisible) ((blockFacesVisible) & TOP_FACE)
#define BOTTOM_FACE_VISIBLE(blockFacesVisible) ((blockFacesVisible) & BOTTOM_FACE)
#define NORTH_FACE_VISIBLE(blockFacesVisible) ((blockFacesVisible) & NORTH_FACE)
#define SOUTH_FACE_VISIBLE(blockFacesVisible) ((blockFacesVisible) & SOUTH_FACE)
#define EAST_FACE_VISIBLE(blockFacesVisible) ((blockFacesVisible) & EAST_FACE)
#define WEST_FACE_VISIBLE(blockFacesVisible) ((blockFacesVisible) & WEST_FACE)

namespace engine
{
    // forward declaration of chunk
    class chunk;

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
                Vertex() = default;

                glm::vec3 position;
                glm::vec3 normal;
                glm::vec2 uv;

                static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
                static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

                bool operator==(const Vertex& other) const
                {
                    return position == other.position && uv == other.uv && normal == other.normal;
                }
            };

            block(int x, int y, int z, BlockType blockType) : pos{x, y, z}, blockFacesVisible(0b111111), blockType(blockType) {};
            block() = default;
            ~block() = default;

            void updateBlockFacesVisible(std::vector<std::shared_ptr<chunk>>& chunks);
            void checkFaceInChunk(std::shared_ptr<chunk>& chunk);

            void addVerticesToBuffer(
                std::unordered_map<Vertex, uint32_t>& uniqueVertices, 
                std::vector<Vertex>& vertices, 
                std::vector<uint32_t>& indices);

        private:
            int pos[3];
            unsigned char blockFacesVisible;
            BlockType blockType; 

            static void pushVertex(
                std::unordered_map<Vertex, uint32_t>& uniqueVertices, 
                std::vector<Vertex>& vertices,
                std::vector<uint32_t>& indices, 
                const block::Vertex& vertex);

    };
}

