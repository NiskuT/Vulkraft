#pragma once

#include "gameWindow.hpp"
#include "engineDevice.hpp"
#include "swapChain.hpp"


#include <memory>
#include <vector>
#include <cassert>

namespace engine 
{
    class renderer
    {
        public:

            renderer(gameWindow& window, engineDevice& device);
            ~renderer();

            renderer(const renderer&) = delete;
            void operator=(const renderer&) = delete;

            VkRenderPass getSwapChainRenderPass() const { return p_swapChain->getRenderPass(); }
            bool isFrameInProgress() const { return isFrameStarted; }
            
            VkCommandBuffer getCurrentCommandBuffer() const { 
                assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
                return commandBuffers[currentFrameIndex]; 
            }

            VkCommandBuffer beginFrame();
            void endFrame();
            void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
            void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

            int getFrameIndex() const { 
                assert(isFrameStarted && "Cannot get frame index when frame not in progress");
                return currentFrameIndex; 
            }

        private:
            void createCommandBuffers();
            void freeCommandBuffers();
            void recreateSwapChain();

            gameWindow& window;
            engineDevice& device;
            std::unique_ptr<swapChain> p_swapChain;
            std::vector<VkCommandBuffer> commandBuffers;    

            uint32_t currentImageIndex;
            int currentFrameIndex{0};
            bool isFrameStarted{false}; 
    };
}