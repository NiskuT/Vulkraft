#pragma once

#include "engineDevice.hpp"
#include "vulkanBuffer.hpp"

#include <stb_image.h>

#include <vector>
#include <memory>

namespace engine
{
    class engineTexture
    {
    public:
        engineTexture(engineDevice& device, stbi_uc* pixels, int texWidth, int texHeight);
        ~engineTexture();

        engineTexture(const engineTexture&) = delete;
        void operator=(const engineTexture&) = delete;

        static std::unique_ptr<engineTexture> createTextureFromFile(engineDevice& device, const std::string& filepath);

        VkDescriptorImageInfo* getDescriptor() { return &descriptor; }

    private:
        engineDevice& device;
        void createTextureImage(int texWidth, int texHeight);
        void createTextureImageView();
        void createTextureSampler();
        void setDescriptorImageInfo();

        VkImage image;
        VkDeviceMemory imageMemory;
        VkImageView imageView;
        VkSampler sampler;
        VkDescriptorImageInfo descriptor;
    };


} // namespace engine