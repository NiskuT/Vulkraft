#include "app.hpp"

#include "systems/renderSystem.hpp"
#include "systems/pointLightSystem.hpp"
#include "systems/renderWorld.hpp"
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
#include <iostream>

#ifndef TEXTURE_PATH
  #define TEXTURE_PATH "textures/"
#endif
#ifndef MODEL_PATH
  #define MODEL_PATH "models/"
#endif

#define MAX_FRAME_TIME 0.05f

namespace engine 
{
    app::app()
    {
        std::cout << "path to model: " << MODEL_PATH << std::endl;
        globalPool = engineDescriptorPool::Builder(device)
            .setMaxSets(swapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, swapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, swapChain::MAX_FRAMES_IN_FLIGHT)
            .build();

        blockPool = engineDescriptorPool::Builder(device)
            .setMaxSets(swapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, swapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, swapChain::MAX_FRAMES_IN_FLIGHT)
            .build();

        loadGameObjects();
        loadTextures();
        myWorld = std::make_shared<visibleWorld>(device, 1);

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

        auto globalSetLayout = engineDescriptorSetLayout::Builder(device)
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
            .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
            .build();

        std::vector<VkDescriptorSet> globalDescriptorSets(swapChain::MAX_FRAMES_IN_FLIGHT);
        std::vector<VkDescriptorSet> blockDescriptorSet(swapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); i++)
        {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            auto imageInfo = textures->getDescriptor();
            auto imageBlockInfo = blockTexture->getDescriptor();

            engineDescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .writeImage(1, imageInfo)
                .build(globalDescriptorSets[i]);

            engineDescriptorWriter(*globalSetLayout, *blockPool)
                .writeBuffer(0, &bufferInfo)
                .writeImage(1, imageBlockInfo)
                .build(blockDescriptorSet[i]);
        }

        renderSystem renderSystem{device, engineRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
        renderWorld worldRenderer{device, engineRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
        pointLightSystem pointLightSystem{device, engineRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};

        camera myCamera{};

        auto &viewerObjectPlayer = gameObjects.at(0); // the viewer object = the player  : so we can move the player in the scene 
        keyboardController controller{}; // for the player and the camera

        auto currentTime = std::chrono::high_resolution_clock::now();
        
        while(!window.shouldClose())
        {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            frameTime = std::min(frameTime, MAX_FRAME_TIME);

            controller.moveInPlaneXZ(window.getGLFWwindow(), frameTime, viewerObjectPlayer); // to move the player 
            glm::vec3 pos = controller.computePosCam(viewerObjectPlayer.transform, myCamera, frameTime, window.getGLFWwindow()); 
            if(myCamera.getViewBool()) {
                myCamera.setViewDirection(pos, glm::vec3( sin(viewerObjectPlayer.transform.rotation.y) *cos(myCamera.getAlpha()), 0.0f -sin(myCamera.getAlpha()) , cos(viewerObjectPlayer.transform.rotation.y)*cos(myCamera.getAlpha()))) ; 
            } else {
                myCamera.setViewTarget(pos, viewerObjectPlayer.transform.translation + glm::vec3(0.0f, -0.5f, 0.0f)); 
            }
            
            float aspect = engineRenderer.getAspectRatio();

            myCamera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
            myCamera.setPerspectiveProjection(glm::radians(45.f), aspect, 0.1f, 100.f);

            if (auto commandBuffer = engineRenderer.beginFrame())
            {
                int frameIndex = engineRenderer.getFrameIndex();
                FrameInfo frameInfo{frameIndex, frameTime, commandBuffer, myCamera, globalDescriptorSets[frameIndex], gameObjects, *myWorld};
                FrameInfo frameInfoBlocks{frameIndex, frameTime, commandBuffer, myCamera, blockDescriptorSet[frameIndex], gameObjects, *myWorld};

                // update
                GlobalUbo ubo{};
                ubo.projection = myCamera.getProjectionMatrix();
                ubo.view = myCamera.getViewMatrix();
                ubo.viewInverse = myCamera.getViewInverseMatrix();
                pointLightSystem.update(frameInfo, ubo);
                auto position = myCamera.getPosition();
                myWorld->updateWorldMesh(static_cast<int>(position.x * 10), static_cast<int>(position.z * 10), 4); // x, z, render distance
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                //render
                engineRenderer.beginSwapChainRenderPass(commandBuffer);

                // order matters
                renderSystem.renderGameObjects(frameInfo);
                pointLightSystem.render(frameInfo);
                worldRenderer.renderVisibleWorld(frameInfoBlocks);
                
                engineRenderer.endSwapChainRenderPass(commandBuffer);
                engineRenderer.endFrame();

            }
        }
    }

    void app::loadGameObjects()
    {
        //we create the first object of the scene : the player 
        std::shared_ptr<engineModel> SteveModel = engineModel::createModelFromFile(device, MODEL_PATH "smallSteveWithTexture.obj");
        auto Steve = gameObject::createGameObject();
        Steve.model = SteveModel;
        // first transformations on steve in order to place him in the scene 
        Steve.transform.translation = {0.0f, -0.2f, 1.0f};
        Steve.transform.rotation = {0,0,glm::radians(180.0f)}; 
        Steve.transform.scale = glm::vec3(0.15f);
        //place him in the scene
        gameObjects.emplace(Steve.getId(), std::move(Steve));

        /*gameObjModel = engineModel::createModelFromFile(device, MODEL_PATH "quad.obj");
        auto floor = gameObject::createGameObject();
        floor.model = FloorModel;
        floor.transform.translation = {0.f, .5f, 0.f};
        floor.transform.scale = {3.0f, 1.0f, 3.0f};
        gameObjects.emplace(floor.getId(), std::move(floor));*/


        auto sun = gameObject::makePointLight(2.0f, 3.0f, {1.0f, 1.0f, 0.5f});
        sun.transform.translation = glm::vec3(0.f, 0.0f, 30.0f);
        gameObjects.emplace(sun.getId(), std::move(sun));

        auto moon = gameObject::makePointLight(2.0f, 1.0f, {0.7f, 0.7f, 1.0f});
        moon.transform.translation = glm::vec3(0.f, 0.0f, -30.0f); // opposite positions of start for sun and moon
        gameObjects.emplace(moon.getId(), std::move(moon));

    }

    void app::loadTextures()
    {
        auto textureFilepath = TEXTURE_PATH "TextureSteve.png";
        textures = engineTexture::createTextureFromFile(device, textureFilepath);

        auto texturePath = TEXTURE_PATH "blockTextureAlpha.png";
        blockTexture = engineTexture::createTextureFromFile(device, texturePath);
    }


} // namespace engine