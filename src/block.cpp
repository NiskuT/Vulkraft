#include "block.hpp"
#include "chunk.hpp"

#include <glm/gtx/hash.hpp>

#include "utils.hpp"
#include <iostream>

namespace engine
{
    
    void block::addVerticesToBuffer(
        std::unordered_map<Vertex, uint32_t>& uniqueVertices, 
        std::vector<Vertex>& vertices, 
        std::vector<uint32_t>& indices)
    {
        float worldX, worldY, worldZ;
        worldX = static_cast<float>(pos[0]);
        worldY = static_cast<float>(pos[1]);
        worldZ = static_cast<float>(pos[2]);
        
        constexpr float txUnitX = 1.f / 3.0f;
        constexpr float txUnitY = 1.f / static_cast<float>(BlockType::NUMBER_OF_BLOCKS);

        float textureY = static_cast<float>(blockType) * txUnitY;

        if (TOP_FACE_VISIBLE(blockFacesVisible) != 0)
        {
            auto norm = glm::vec3{0.f, -1.f, 0.f};
            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY, worldZ}, norm, {TOP_TX * txUnitX, textureY}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY, worldZ}, norm, {1.0f, textureY}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY, worldZ + 1.f}, norm, {TOP_TX * txUnitX, textureY + txUnitY}});

            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY, worldZ}, norm, {1.0f, textureY}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY, worldZ + 1.f}, norm, {TOP_TX * txUnitX, textureY + txUnitY}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY, worldZ + 1.f}, norm, {1.0f, textureY + txUnitY}});
        }
        if (BOTTOM_FACE_VISIBLE(blockFacesVisible) != 0)
        {
            auto norm = glm::vec3{0.f, 1.f, 0.f};
            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY + 1.f, worldZ}, norm, {0.f, textureY}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY + 1.f, worldZ}, norm, {txUnitX, textureY}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY + 1.f, worldZ + 1.f}, norm, {0.f, textureY + txUnitY}});

            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY + 1.f, worldZ}, norm, {txUnitX, textureY}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY + 1.f, worldZ + 1.f}, norm, {0.f, textureY + txUnitY}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY + 1.f, worldZ + 1.f}, norm, {txUnitX, textureY + txUnitY}});
        }
        if (NORTH_FACE_VISIBLE(blockFacesVisible) != 0)
        {
            auto norm = glm::vec3{1.f, 0.f, 0.f};
            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY, worldZ}, norm, {SIDE_TX * txUnitX, textureY}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY, worldZ + 1.f}, norm, {SIDE_TX * txUnitX + txUnitX, textureY}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY + 1.f, worldZ}, norm, {SIDE_TX * txUnitX, textureY + txUnitY}});

            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY, worldZ + 1.f}, norm, {SIDE_TX * txUnitX + txUnitX, textureY}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY + 1.f, worldZ}, norm, {SIDE_TX * txUnitX, textureY + txUnitY}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY + 1.f, worldZ + 1.f}, norm, {SIDE_TX * txUnitX + txUnitX, textureY + txUnitY}});
        }
        if (SOUTH_FACE_VISIBLE(blockFacesVisible) != 0)
        {
            auto norm = glm::vec3{-1.f, 0.f, 0.f};
            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY, worldZ}, norm, {SIDE_TX * txUnitX, textureY}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY, worldZ + 1.f}, norm, {SIDE_TX * txUnitX + txUnitX, textureY}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY + 1.f, worldZ}, norm, {SIDE_TX * txUnitX, textureY + txUnitY}});

            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY, worldZ + 1.f}, norm, {SIDE_TX * txUnitX + txUnitX, textureY}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY + 1.f, worldZ}, norm, {SIDE_TX * txUnitX, textureY + txUnitY}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY + 1.f, worldZ + 1.f}, norm, {SIDE_TX * txUnitX + txUnitX, textureY + txUnitY}});
        }
        if (EAST_FACE_VISIBLE(blockFacesVisible) != 0)
        {
            auto norm = glm::vec3{0.f, 0.f, -1.f};
            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY, worldZ}, norm, {SIDE_TX * txUnitX, textureY}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY, worldZ}, norm, {SIDE_TX * txUnitX + txUnitX, textureY}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY + 1.f, worldZ}, norm, {SIDE_TX * txUnitX, textureY + txUnitY}});

            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY, worldZ}, norm, {SIDE_TX * txUnitX + txUnitX, textureY}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY + 1.f, worldZ}, norm, {SIDE_TX * txUnitX, textureY + txUnitY}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY + 1.f, worldZ}, norm, {SIDE_TX * txUnitX + txUnitX, textureY + txUnitY}});
        }
        if (WEST_FACE_VISIBLE(blockFacesVisible) != 0)
        {
            auto norm = glm::vec3{0.f, 0.f, +1.f};
            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY, worldZ + 1.f}, norm, {SIDE_TX * txUnitX, textureY}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY, worldZ + 1.f}, norm, {SIDE_TX * txUnitX + txUnitX, textureY}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY + 1.f, worldZ + 1.f}, norm, {SIDE_TX * txUnitX, textureY + txUnitY}});

            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY, worldZ + 1.f}, norm, {SIDE_TX * txUnitX + txUnitX, textureY}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY + 1.f, worldZ + 1.f}, norm, {SIDE_TX * txUnitX, textureY + txUnitY}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY + 1.f, worldZ + 1.f}, norm, {SIDE_TX * txUnitX + txUnitX, textureY + txUnitY}});
        }
    }

    void block::pushVertex(
        std::unordered_map<Vertex, uint32_t>& uniqueVertices, 
        std::vector<Vertex>& vertices,
        std::vector<uint32_t>& indices, 
        const block::Vertex& vertex)
    {  
        if (uniqueVertices.count(vertex) == 0)
        {
            uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
            vertices.push_back(vertex);
        }
        indices.push_back(uniqueVertices[vertex]);
    }

    std::vector<VkVertexInputBindingDescription> block::Vertex::getBindingDescriptions()
    {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0; 
        bindingDescriptions[0].stride = sizeof(block::Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> block::Vertex::getAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

        attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(block::Vertex, position)});
        attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(block::Vertex, normal)});
        attributeDescriptions.push_back({2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(block::Vertex, uv)});

        return attributeDescriptions;
    }

    int findChunkIndex(std::vector<std::shared_ptr<chunk>>& chunks, int targetX, int targetY) {
        for (size_t i = 0; i < chunks.size(); ++i) {
            int posX, posY;
            chunks[i]->getPosition(posX, posY);
            if (posX == targetX && posY == targetY) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    void block::checkFaceInChunk(std::shared_ptr<chunk>& chunk)
    {
        if (chunk->isThereABlockAt(pos[0] - 1, pos[1], pos[2]))
        {
            blockFacesVisible &= ~SOUTH_FACE;
        }
        if (chunk->isThereABlockAt(pos[0] + 1, pos[1], pos[2]))
        {
            blockFacesVisible &= ~NORTH_FACE;
        }
        if (chunk->isThereABlockAt(pos[0], pos[1] + 1, pos[2]))
        {
            blockFacesVisible &= ~BOTTOM_FACE;
        }
        if (chunk->isThereABlockAt(pos[0], pos[1] - 1, pos[2]))
        {
            blockFacesVisible &= ~TOP_FACE;
        }
        if (chunk->isThereABlockAt(pos[0], pos[1], pos[2] - 1))
        {
            blockFacesVisible &= ~EAST_FACE;
        }
        if (chunk->isThereABlockAt(pos[0], pos[1], pos[2] + 1))
        {
            blockFacesVisible &= ~WEST_FACE;
        }
    }

    void block::updateBlockFacesVisible(std::vector<std::shared_ptr<chunk>>& chunks)
    {
        blockFacesVisible = 0b111111;

        int myChunkX = pos[0] / CHUNK_SIZE;
        int myChunkZ = pos[2] / CHUNK_SIZE;

        int tmpX = pos[0], tmpZ = pos[2];

        if (pos[0] < 0) 
        {
            tmpX += 1;
            myChunkX = tmpX / CHUNK_SIZE;
            myChunkX -= 1;
        }
        if (pos[2] < 0)
        {
            tmpZ += 1;
            myChunkZ = tmpZ / CHUNK_SIZE;
            myChunkZ -= 1;
        }

        // In case of water, we only need to check the top face, if it's under water we don't need to check anything else
        if (blockType == BlockType::WATER)
        {
            blockFacesVisible = 0b000000;
            auto id = findChunkIndex(chunks, myChunkX, myChunkZ);
            if (!chunks[id]->isThereAWaterBlockAt(pos[0], pos[1] - 1, pos[2]))
            {
                blockFacesVisible = TOP_FACE;
            }
            return;
        }

        // We first check that the block is note on the edge of the chunk

        // NEGATIVE CASE
        if (pos[0] < 0 && tmpX % CHUNK_SIZE == 0) {
            auto id = findChunkIndex(chunks, myChunkX + 1, myChunkZ);
            if (id != -1) {
                checkFaceInChunk(chunks[id]);
            }
        }
        if (pos[0] < 0 && tmpX % CHUNK_SIZE == -CHUNK_SIZE + 1) {
            auto id = findChunkIndex(chunks, myChunkX - 1, myChunkZ);
            if (id != -1) {
                checkFaceInChunk(chunks[id]);
            }
        }
        if (pos[2] < 0 && tmpZ % CHUNK_SIZE == 0) {
            auto id = findChunkIndex(chunks, myChunkX, myChunkZ + 1);
            if (id != -1) {
                checkFaceInChunk(chunks[id]);
            }
        }
        if (pos[2] < 0 && tmpZ % CHUNK_SIZE == -CHUNK_SIZE + 1) {
            auto id = findChunkIndex(chunks, myChunkX, myChunkZ - 1);
            if (id != -1) {
                checkFaceInChunk(chunks[id]);
            }
        }

        // POSITIVE CASE
        if (pos[0] > 0 && tmpX % CHUNK_SIZE == 0) {
            auto id = findChunkIndex(chunks, myChunkX - 1, myChunkZ);
            if (id != -1) {
                checkFaceInChunk(chunks[id]);
            }
        }
        if (pos[0] > 0 && tmpX % CHUNK_SIZE == CHUNK_SIZE - 1) {
            auto id = findChunkIndex(chunks, myChunkX + 1, myChunkZ);
            if (id != -1) {
                checkFaceInChunk(chunks[id]);
            }
        }
        if (pos[2] > 0 && tmpZ % CHUNK_SIZE == 0) {
            auto id = findChunkIndex(chunks, myChunkX, myChunkZ - 1);
            if (id != -1) {
                checkFaceInChunk(chunks[id]);
            }
        }
        if (pos[2] > 0 && tmpZ % CHUNK_SIZE == CHUNK_SIZE - 1) {
            auto id = findChunkIndex(chunks, myChunkX, myChunkZ + 1);
            if (id != -1) {
                checkFaceInChunk(chunks[id]);
            }
        }

        // General case
        auto id = findChunkIndex(chunks, myChunkX, myChunkZ);
        if (id != -1) {
            checkFaceInChunk(chunks[id]);
        }

    }


} // namespace engine
