#pragma once

#include "engineDevice.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace engine
{
    /*
     *   The purpose of this class is to be able to take vertex data created by or read in a file by the CPU
     *   and then allocate the memory and copy the data to the GPU in order to render it efficiently.
     */
    class engineModel
    {
    public:
        struct Vertex
        {
            glm::vec2 position;
            glm::vec3 color;

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };
        engineModel(engineDevice& device, const std::vector<Vertex>& vertices);
        ~engineModel();

        engineModel(const engineModel&) = delete;
        void operator=(const engineModel&) = delete;

        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer);

    private:
        engineDevice& device;
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        uint32_t vertexCount;

        void createVertexBuffers(const std::vector<Vertex>& vertices);
    };
    
} // namespace engine

