#include "app.hpp"

#include "renderSystem.hpp"
#include "camera.hpp"
#include "keyboardController.hpp"
#include "vulkanBuffer.hpp"

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
    struct GlobalUbo
    {
        glm::mat4 projectionView{1.0f};
        glm::vec3 lightDirection = glm::normalize(glm::vec3(1.0f, -3.0f, -1.0f));
    };
    
    app::app()
    {
        loadGameObjects();
    }

    app::~app() {}
    
    void app::run()
    {
        std::vector<std::unique_ptr<vulkanBuffer>> uboBuffers(swapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i =0; i< uboBuffers.size(); i++)
        {
            uboBuffers[i] = std::make_unique<vulkanBuffer>(
                device,
                sizeof(GlobalUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
            );
            uboBuffers[i]->map();
        }

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
                int frameIndex = engineRenderer.getFrameIndex();
                FrameInfo frameInfo{frameIndex, frameTime, commandBuffer, myCamera};

                // update
                GlobalUbo ubo{};
                ubo.projectionView = myCamera.getProjectionMatrix() * myCamera.getViewMatrix();
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                //render
                engineRenderer.beginSwapChainRenderPass(commandBuffer);
                renderSystem.renderGameObjects(frameInfo, gameObjects);
                engineRenderer.endSwapChainRenderPass(commandBuffer);
                engineRenderer.endFrame();

            }
        }
    }

    void app::loadGameObjects()
    {
        std::shared_ptr<engineModel> gameObjModel = engineModel::createModelFromFile(device, "C:\\Users\\qjupi\\Desktop\\Vulkraft\\models\\flat_vase.obj");
        auto gameObj = gameObject::createGameObject();
        gameObj.model = gameObjModel;

        gameObj.transform.translation = {0.0f, 0.5f, 2.5f};
        gameObj.transform.scale = glm::vec3(3.0f, 2.0f, 1.0f);

        gameObjects.push_back(std::move(gameObj));
    }
} // namespace engine