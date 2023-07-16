#include "engineTexture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <stdexcept>

namespace engine
{
    std::unique_ptr<engineTexture> engineTexture::createTextureFromFile(engineDevice& device, const std::string& filepath)
    {
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load(filepath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

        if (!pixels) {
            throw std::runtime_error("échec du chargement d'une image!");
        }

        auto texture = std::make_unique<engineTexture>(device, pixels, texWidth, texHeight);

        stbi_image_free(pixels);

        return texture;
    }

    engineTexture::engineTexture(engineDevice& device, stbi_uc* pixels, int texWidth, int texHeight) : device{device}
    {
        createTextureImage(texWidth, texHeight);

        vulkanBuffer stagingBuffer(
            device, 
            4,  // 4 bytes per pixel
            texWidth * texHeight,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        stagingBuffer.map();
        stagingBuffer.writeToBuffer((void *)pixels);

        device.transitionImageLayout(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        device.copyBufferToImage(stagingBuffer.getBuffer(), image, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1);
        device.transitionImageLayout(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        createTextureImageView();
        createTextureSampler();
        setDescriptorImageInfo();

    }

    engineTexture::~engineTexture()
    {
        vkDestroySampler(device.device(), sampler, nullptr);
        vkDestroyImageView(device.device(), imageView, nullptr);
        vkDestroyImage(device.device(), image, nullptr);
        vkFreeMemory(device.device(), imageMemory, nullptr);
    }

    void engineTexture::createTextureImage(int texWidth, int texHeight)
    {
        // Parameters for the image
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = static_cast<uint32_t>(texWidth);
        imageInfo.extent.height = static_cast<uint32_t>(texHeight);
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;  // 4 bytes per pixel
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;  // Not usable by the GPU and the first transition will discard the texels
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;  // Transfer destination because we'll copy the buffer to it, and sampled because it will be accessible to shaders
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.flags = 0;  // Optional

        device.createImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, imageMemory);
    }

    void engineTexture::createTextureImageView()
    {
        // Parameters for the image view
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;  // Treat the image as a 1D texture
        viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;  // Treat the image's color data as the color attachment
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;  // No mipmapping
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device.device(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture image view!");
        }
    }

    void engineTexture::createTextureSampler()
    {
        // Parameters for the sampler
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;  // interpolate texels that are magnified on the screen
        samplerInfo.minFilter = VK_FILTER_LINEAR;  // interpolate texels that are minified on the screen
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;  // How to handle texture coordinates outside of the [0, 1] range in the U direction
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;  // Enable anisotropic filtering
        samplerInfo.maxAnisotropy = 16;  // Anisotropy level
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK; 
        samplerInfo.unnormalizedCoordinates = VK_FALSE; 
        samplerInfo.compareEnable = VK_FALSE; 
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS; 
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;  // Mipmap filter to use
        samplerInfo.mipLodBias = 0.0f;  
        samplerInfo.minLod = 0.0f;  
        samplerInfo.maxLod = 0.0f; 

        if (vkCreateSampler(device.device(), &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }
    
    void engineTexture::setDescriptorImageInfo()
    {
        descriptor.sampler = sampler;
        descriptor.imageView = imageView;
        descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }
}