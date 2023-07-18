#pragma once

#include "pipeline.hpp"
#include "engineDevice.hpp"
#include "camera.hpp"
#include "frameInfo.hpp"

#include <memory>
#include <vector>

namespace engine 
{
    class renderWorld
    {
        public:

            renderWorld(engineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
            ~renderWorld();

            renderWorld(const renderWorld&) = delete;
            void operator=(const renderWorld&) = delete;

            void renderVisibleWorld(FrameInfo& frameInfo);

        private:
            void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
            void createPipeline(VkRenderPass renderPass);
            

            engineDevice& device;
            std::unique_ptr<pipeline> p_pipeline;
            VkPipelineLayout pipelineLayout;    
            std::shared_ptr<engineModel> gameObjModel;  
    };
}