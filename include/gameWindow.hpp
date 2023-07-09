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

            const int width;
            const int height;

            std::string windowName;
            GLFWwindow* window;
        
        public:
            gameWindow(int width, int height, std::string name);
            ~gameWindow();

            gameWindow(const gameWindow &) = delete;
            gameWindow &operator=(const gameWindow &) = delete;

            bool shouldClose() { return glfwWindowShouldClose(window); }
        };
    
} // namespace GameWindow