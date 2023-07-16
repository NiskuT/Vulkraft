#pragma once

#include "block.hpp"

#include <unordered_map>
#include <vector>
#include <memory>

#define CHUNK_SIZE 16

namespace engine
{
    class chunk
    {
        public:
            chunk(int x, int y);
            ~chunk() = default;

            void addBlock(int worldX, int worldY, int worldZ, BlockType blockType);
            void updateBlockFacesVisible(std::vector<std::shared_ptr<chunk>>& chunks);
            bool isThereABlockAt(int worldX, int worldY, int worldZ);

            void getPosition(int& x, int& y) { x = this->x; y = this->y; };

            void getChunkMesh(
                std::unordered_map<block::Vertex, uint32_t>& uniqueVertices, 
                std::vector<block::Vertex>& vertices, 
                std::vector<uint32_t>& indices);

        private:
            unsigned long int hashFunction(int x, int y, int z);
            int x, y;
            std::unordered_map<unsigned long int, block> blocks;

    };
}