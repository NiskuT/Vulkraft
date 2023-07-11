#include "app.hpp"

#include "renderSystem.hpp"
#include "camera.hpp"
#include "keyboardController.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <chrono>
#include <array>

#define MAX_FRAME_TIME 0.05f

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
        myCamera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

        auto viewerObject = gameObject::createGameObject();
        keyboardController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();
        
        while(!window.shouldClose())
        {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            frameTime = std::min(frameTime, MAX_FRAME_TIME);

            cameraController.moveInPlaneXZ(window.getGLFWwindow(), frameTime, viewerObject);
            myCamera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

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

    void app::loadGameObjects()
    {
        std::shared_ptr<engineModel> gameObjModel = engineModel::createModelFromFile(device, "C:\\Users\\qjupi\\Desktop\\Vulkraft\\models\\smooth_vase.obj");
        auto gameObj = gameObject::createGameObject();
        gameObj.model = gameObjModel;

        gameObj.transform.translation = {0.0f, 0.0f, 2.5f};
        gameObj.transform.scale = glm::vec3(3.0f);

        gameObjects.push_back(std::move(gameObj));
    }
} // namespace engine