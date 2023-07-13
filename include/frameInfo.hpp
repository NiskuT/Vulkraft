#pragma once

#include "camera.hpp"

#include <vulkan/vulkan.h>

#include "gameObject.hpp"

namespace engine {

    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        camera& camera;
        VkDescriptorSet globalDescriptorSet;
        gameObject::Map& gameObjects;
    };
}  // namespace engine