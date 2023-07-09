#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace engine
{
    class gameWindow
    {
        private:

            void initWindow();

            int width;
            int height;
            bool framebufferResized = false;

            std::string windowName;
            GLFWwindow* window;

            static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
        
        public:
            gameWindow(int width, int height, std::string name);
            ~gameWindow();

            gameWindow(const gameWindow &) = delete;
            gameWindow &operator=(const gameWindow &) = delete;

            bool shouldClose() { return glfwWindowShouldClose(window); }
            void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
            VkExtent2D getExtent() { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }
            bool wasWindowResized() { return framebufferResized; }
            void resetWindowResizedFlag() { framebufferResized = false; }
        };
    
} // namespace GameWindow