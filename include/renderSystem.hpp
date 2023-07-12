#pragma once

#include "pipeline.hpp"
#include "engineDevice.hpp"
#include "gameObject.hpp"
#include "camera.hpp"
#include "frameInfo.hpp"

#include <memory>
#include <vector>

namespace engine 
{
    class renderSystem
    {
        public:

            renderSystem(engineDevice& device, VkRenderPass renderPass);
            ~renderSystem();

            renderSystem(const renderSystem&) = delete;
            void operator=(const renderSystem&) = delete;

            void renderGameObjects(FrameInfo& frameInfo, std::vector<gameObject>& gameObjects);

        private:
            void createPipelineLayout();
            void createPipeline(VkRenderPass renderPass);
            

            engineDevice& device;
            std::unique_ptr<pipeline> p_pipeline;
            VkPipelineLayout pipelineLayout;      
    };
}