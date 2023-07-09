#include "gameWindow.hpp"

#include <stdexcept>

namespace engine
{
    gameWindow::gameWindow(int width, int height, std::string name) : width(width), height(height), windowName(name)
    {
        initWindow();
    }

    gameWindow::~gameWindow()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void gameWindow::initWindow()
    {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    }

    void gameWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
    {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create window surface!");
        }
    }
} // namespace engine
