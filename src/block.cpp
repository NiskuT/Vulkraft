#include "block.hpp"

#include "utils.hpp"

#include <glm/gtx/hash.hpp>
#include 


namespace std
{
    template <>
    struct hash<engine::block::Vertex>
    {
        size_t operator()(engine::block::Vertex const& vertex) const
        {
            size_t seed = 0;
            engine::hashCombine(seed, vertex.position, vertex.uv);
            return seed;
        }
    };
}

namespace engine
{
    
    void block::addVerticesToBuffer(
        std::unordered_map<Vertex, uint32_t>& uniqueVertices, 
        std::vector<Vertex>& vertices, 
        std::vector<uint32_t>& indices, 
        int chunkX, int chunkY)
    {
        float worldX, worldY, worldZ;
        worldX = static_cast<float>(pos[0] + chunkX * 16);
        worldY = static_cast<float>(pos[1] + chunkY * 16);
        worldZ = static_cast<float>(pos[2]);
        if (TOP_FACE_VISIBLE(blockFacesVisible))
        {
            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY, worldZ}, {0.0f, 0.0f}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY, worldZ}, {0.0f, 1.0f}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY + 1.f, worldZ}, {0.0f, 1.0f}});

            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY + 1.f, worldZ}, {0.0f, 1.0f}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY, worldZ}, {0.0f, 1.0f}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY + 1.f, worldZ}, {0.0f, 1.0f}});
        }
        if (BOTTOM_FACE_VISIBLE(blockFacesVisible))
        {
            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY, worldZ - 1.f}, {0.0f, 0.0f}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY, worldZ - 1.f}, {0.0f, 1.0f}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY + 1.f, worldZ - 1.f}, {0.0f, 1.0f}});

            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY + 1.f, worldZ - 1.f}, {0.0f, 1.0f}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY, worldZ - 1.f}, {0.0f, 1.0f}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY + 1.f, worldZ - 1.f}, {0.0f, 1.0f}});
        }
        if (NORTH_FACE_VISIBLE(blockFacesVisible))
        {
            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY + 1.f, worldZ}, {0.0f, 0.0f}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY + 1.f, worldZ - 1.f}, {0.0f, 1.0f}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY + 1.f, worldZ - 1.f}, {0.0f, 1.0f}});

            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY + 1.f, worldZ}, {0.0f, 1.0f}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY + 1.f, worldZ - 1.f}, {0.0f, 1.0f}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY + 1.f, worldZ}, {0.0f, 1.0f}});
        }
        if (SOUTH_FACE_VISIBLE(blockFacesVisible))
        {
            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY, worldZ}, {0.0f, 0.0f}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY, worldZ - 1.f}, {0.0f, 1.0f}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY, worldZ - 1.f}, {0.0f, 1.0f}});

            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY, worldZ}, {0.0f, 1.0f}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY, worldZ - 1.f}, {0.0f, 1.0f}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY, worldZ}, {0.0f, 1.0f}});
        }
        if (EAST_FACE_VISIBLE(blockFacesVisible))
        {
            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY, worldZ}, {0.0f, 0.0f}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY, worldZ - 1.f}, {0.0f, 1.0f}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY + 1.f, worldZ - 1.f}, {0.0f, 1.0f}});

            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY, worldZ}, {0.0f, 1.0f}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY + 1.f, worldZ - 1.f}, {0.0f, 1.0f}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX + 1.f, worldY + 1.f, worldZ}, {0.0f, 1.0f}});
        }
        if (WEST_FACE_VISIBLE(blockFacesVisible))
        {
            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY + 1.f, worldZ}, {0.0f, 0.0f}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY + 1.f, worldZ - 1.f}, {0.0f, 1.0f}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY, worldZ - 1.f}, {0.0f, 1.0f}});

            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY + 1.f, worldZ}, {0.0f, 1.0f}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY, worldZ - 1.f}, {0.0f, 1.0f}});
            pushVertex(uniqueVertices, vertices, indices, {{worldX, worldY, worldZ}, {0.0f, 1.0f}});
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

} // namespace engine
