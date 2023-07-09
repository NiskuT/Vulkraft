#pragma once

#include "engineDevice.hpp"

#include <string>
#include <vector>

namespace engine
{

    struct pipelineConfigInfo
    {
        VkViewport viewport;
        VkRect2D scissor;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };
    
    class pipeline
    {
        private:
            static std::vector<char> readFile(const std::string& filepath);

            engineDevice& device;
            VkPipeline graphicsPipeline;
            VkShaderModule vertShaderModule;
            VkShaderModule fragShaderModule;

            void createGraphicsPipeline(
                const std::string& vertFilepath, 
                const std::string& fragFilepath, 
                const pipelineConfigInfo& configInfo);

            void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

        public:
            pipeline(
                engineDevice& device, 
                const std::string& vertFilepath, 
                const std::string& fragFilepath, 
                const pipelineConfigInfo& configInfo);

            ~pipeline();

            pipeline(const pipeline&) = delete;
            void operator=(const pipeline&) = delete;

            void bind(VkCommandBuffer commandBuffer);
            static pipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);
    };
    
} // namespace engine