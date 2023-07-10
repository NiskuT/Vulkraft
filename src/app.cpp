#include "app.hpp"

#include "renderSystem.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>

namespace engine 
{
    
    app::app()
    {
        loadGameObjects();
    }

    app::~app() {}
    
    void app::run()
    {
        renderSystem renderSystem{device, engineRenderer.getSwapChainRenderPass()};

        while(!window.shouldClose())
        {
            glfwPollEvents();
            
            if (auto commandBuffer = engineRenderer.beginFrame())
            {
                engineRenderer.beginSwapChainRenderPass(commandBuffer);
                renderSystem.renderGameObjects(commandBuffer, gameObjects);
                engineRenderer.endSwapChainRenderPass(commandBuffer);
                engineRenderer.endFrame();

            }
        }
    }

    void app::loadGameObjects()
    {
        std::vector<engineModel::Vertex> vertices{
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
        };

        auto model = std::make_shared<engineModel>(device, vertices);

        auto triangle = gameObject::createGameObject();
        triangle.model = model;
        triangle.color = {.1f, .8f, .1f};
        triangle.transform2d.translation.x = .2f;
        triangle.transform2d.scale = {2.0f, .5f};
        triangle.transform2d.rotation = .25f * glm::two_pi<float>();

        gameObjects.push_back(std::move(triangle));

    }
} // namespace engine