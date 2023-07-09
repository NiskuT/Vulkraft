#pragma once

#include "engineDevice.hpp"

#include <string>
#include <vector>

namespace engine
{

    struct pipelineConfigInfo
    {
        pipelineConfigInfo() = default;
        pipelineConfigInfo(const pipelineConfigInfo&) = delete;
        pipelineConfigInfo operator=(const pipelineConfigInfo&) = delete;

        VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        std::vector<VkDynamicState> dynamicStateEnables;
        VkPipelineDynamicStateCreateInfo dynamicStateInfo;
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
            pipeline() = default;

            pipeline(const pipeline&) = delete;
            pipeline& operator=(const pipeline&) = delete;

            void bind(VkCommandBuffer commandBuffer);
            static void defaultPipelineConfigInfo(pipelineConfigInfo& configInfo);
    };
    
} // namespace engine