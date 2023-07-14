#pragma once

#include "camera.hpp"

#include <vulkan/vulkan.h>

#include "gameObject.hpp"

namespace engine {

#define MAX_LIGHTS 10

    struct PointLight {
        glm::vec4 position{}; // ignore w
        glm::vec4 color{}; // w is light intensity
    };
    struct GlobalUbo
    {
        glm::mat4 projection{1.0f};
        glm::mat4 view{1.0f};
        glm::mat4 viewInverse{1.0f};
        glm::vec4 ambientLightColor{0.1f, 0.1f, 0.1f, 0.02f}; // w is the ambient light intensity
        PointLight pointLights[MAX_LIGHTS];
        int numLights;
    };

    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        camera& myCamera;
        VkDescriptorSet globalDescriptorSet;
        gameObject::Map& gameObjects;
    };

}  // namespace engine