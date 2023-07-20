#pragma once

#include "pipeline.hpp"
#include "engineDevice.hpp"
#include "gameObject.hpp"
#include "camera.hpp"
#include "frameInfo.hpp"

#include <memory>
#include <vector>

#define MAX_SUN_HEIGHT 30

namespace engine 
{
    class pointLightSystem
    {
        public:

            pointLightSystem(engineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
            ~pointLightSystem();

            pointLightSystem(const pointLightSystem&) = delete;
            void operator=(const pointLightSystem&) = delete;

            void update(FrameInfo& frameInfo, GlobalUbo& ubo);
            void render(FrameInfo& frameInfo);

            static glm::vec3 getSkyColor(float sunHeight);
            float getSunHeight(FrameInfo &frameInfo);

        private:
            void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
            void createPipeline(VkRenderPass renderPass);
            

            engineDevice& device;
            std::unique_ptr<pipeline> p_pipeline;
            VkPipelineLayout pipelineLayout;      
    };
}