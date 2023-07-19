#pragma once

#include "gameWindow.hpp"
#include "engineDevice.hpp"
#include "swapChain.hpp"


#include <memory>
#include <vector>
#include <cassert>

namespace engine 
{
    /*
    Contains the rendering operations : 
    manages creation and destruction of command buffers, swap chain, frames
    */
    class renderer
    {
        public:
            // constructor, destructor, copy...
            renderer(gameWindow& window, engineDevice& device);
            ~renderer();

            renderer(const renderer&) = delete;
            void operator=(const renderer&) = delete;

            // some helper functions of obtain information about the current state
            VkRenderPass getSwapChainRenderPass() const { return p_swapChain->getRenderPass(); }
            float getAspectRatio() const { return p_swapChain->extentAspectRatio(); }
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
            void createCommandBuffers(); // create the command buffers used for rendering 
            void freeCommandBuffers(); // free them
            void recreateSwapChain();

            gameWindow& window;
            engineDevice& device;
            std::unique_ptr<swapChain> p_swapChain; // unique pointer to the Swap chain object
            std::vector<VkCommandBuffer> commandBuffers; // vector holding the command buffers

            uint32_t currentImageIndex;
            int currentFrameIndex{0};
            bool isFrameStarted{false}; 
    };
}