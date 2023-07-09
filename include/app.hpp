#pragma once

#include "gameWindow.hpp"
#include "pipeline.hpp"
#include "engineDevice.hpp"
#include "swapChain.hpp"
#include "engineModel.hpp"


#include <memory>
#include <vector>

namespace engine 
{
    class app
    {
        public:
            static constexpr int WIDTH = 800;
            static constexpr int HEIGHT = 600;

            app();
            ~app();

            app(const app&) = delete;
            void operator=(const app&) = delete;

            void run();

        private:
            gameWindow window{WIDTH, HEIGHT, "Vulkraft"};
            engineDevice device{window};
            swapChain swapChain{device, window.getExtent()};
            void createPipelineLayout();
            void createPipeline();
            void createCommandBuffers();
            void drawFrame();
            void loadModels();

            std::unique_ptr<pipeline> p_pipeline;
            VkPipelineLayout pipelineLayout;
            std::vector<VkCommandBuffer> commandBuffers;  
            std::unique_ptr<engineModel> model;          
    };
}