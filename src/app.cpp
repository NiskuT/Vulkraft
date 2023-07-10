#include "app.hpp"

#include "renderSystem.hpp"
#include "camera.hpp"

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
        camera myCamera{};
        
        while(!window.shouldClose())
        {
            glfwPollEvents();

            float aspect = engineRenderer.getAspectRatio();
            myCamera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
            myCamera.setPerspectiveProjection(glm::radians(45.f), aspect, 0.1f, 10.f);

            if (auto commandBuffer = engineRenderer.beginFrame())
            {
                engineRenderer.beginSwapChainRenderPass(commandBuffer);
                renderSystem.renderGameObjects(commandBuffer, gameObjects, myCamera);
                engineRenderer.endSwapChainRenderPass(commandBuffer);
                engineRenderer.endFrame();

            }
        }
    }

    // temporary helper function, creates a 1x1x1 cube centered at offset
    std::unique_ptr<engineModel> createCubeModel(engineDevice& device, glm::vec3 offset) 
    {
        std::vector<engineModel::Vertex> vertices
        {
            // left face (white)
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
        
            // right face (yellow)
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
        
            // top face (orange, remember y axis points down)
            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
        
            // bottom face (red)
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
        
            // nose face (blue)
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
        
            // tail face (green)
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        
        };
        for (auto& v : vertices) {
            v.position += offset;
        }
        return std::make_unique<engineModel>(device, vertices);
    }

    void app::loadGameObjects()
    {
        std::shared_ptr<engineModel> cubeModel = createCubeModel(device, {0.0f, 0.0f, 0.0f});
        auto cube = gameObject::createGameObject();
        cube.model = cubeModel;

        cube.transform.translation = {0.0f, 0.0f, 2.5f};
        cube.transform.scale = {.5f, .5f, .5f};

        gameObjects.push_back(std::move(cube));
    }
} // namespace engine