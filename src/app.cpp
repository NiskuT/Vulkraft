#include "app.hpp"

#include "systems/renderSystem.hpp"
#include "systems/pointLightSystem.hpp"
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

#define MAX_FRAME_TIME 0.05f

namespace engine 
{
    app::app()
    {
        globalPool = engineDescriptorPool::Builder(device)
            .setMaxSets(swapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, swapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, swapChain::MAX_FRAMES_IN_FLIGHT)
            .build();
        loadGameObjects();
        loadTextures();

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
        for (int i = 0; i < globalDescriptorSets.size(); i++)
        {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            auto imageInfo = textures->getDescriptor();

            engineDescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .writeImage(1, imageInfo)
                .build(globalDescriptorSets[i]);
        }

        renderSystem renderSystem{device, engineRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
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
                FrameInfo frameInfo{frameIndex, frameTime, commandBuffer, myCamera, globalDescriptorSets[frameIndex], gameObjects};

                // update
                GlobalUbo ubo{};
                ubo.projection = myCamera.getProjectionMatrix();
                ubo.view = myCamera.getViewMatrix();
                ubo.viewInverse = myCamera.getViewInverseMatrix();
                pointLightSystem.update(frameInfo, ubo);
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                //render
                engineRenderer.beginSwapChainRenderPass(commandBuffer);

                // order matters
                renderSystem.renderGameObjects(frameInfo);
                pointLightSystem.render(frameInfo);

                
                engineRenderer.endSwapChainRenderPass(commandBuffer);
                engineRenderer.endFrame();

            }
        }
    }

    void app::loadGameObjects()
    {
        //we create the first object of the scene : the player 
        std::shared_ptr<engineModel> SteveModel = engineModel::createModelFromFile(device, "models/smallSteveWithTexture.obj");
        auto Steve = gameObject::createGameObject();
        Steve.model = SteveModel;
        // first transformations on steve in order to place him in the scene 
        Steve.transform.translation = {0.0f, 0.5f, 1.0f};
        Steve.transform.rotation = {0,0,glm::radians(180.0f)}; 
        Steve.transform.scale = glm::vec3(0.3f);
        //place him in the scene
        gameObjects.emplace(Steve.getId(), std::move(Steve));

        // a block 
        std::shared_ptr<engineModel> CubeModel = engineModel::createModelFromFile(device, "models/blocTest.obj");
        auto cube = gameObject::createGameObject();
        cube.model = CubeModel;
        cube.transform.translation = {0.f, -0.1f, 1.7f};
        cube.transform.scale = glm::vec3(0.1f);
        gameObjects.emplace(cube.getId(), std::move(cube));

        // the cubeBox 
        /*std::shared_ptr<engineModel> CubeBoxModel = engineModel::createModelFromFile(device, "models/cubebox.obj");
        auto cubeBox = gameObject::createGameObject();
        cubeBox.model = CubeBoxModel;
        cubeBox.transform.translation = {0.f, 0.f, 0.f};
        cubeBox.transform.scale = glm::vec3(0.3f);
        gameObjects.emplace(cubeBox.getId(), std::move(cubeBox));*/

        //we create the second object of the scene : the floor (flat world for now)
        std::shared_ptr<engineModel> FloorModel = engineModel::createModelFromFile(device, "models/quad.obj");
        auto floor = gameObject::createGameObject();
        floor.model = FloorModel;
        floor.transform.translation = {0.f, .5f, 0.f};
        floor.transform.scale = {3.0f, 1.0f, 3.0f};
        gameObjects.emplace(floor.getId(), std::move(floor));

        // other objects in the scene : the moving point lights 
        std::vector<glm::vec3> lightColors{
            {1.f, .1f, .1f},
            {.1f, .1f, 1.f},
            {.1f, 1.f, .1f},
            {1.f, 1.f, .1f},
            {.1f, 1.f, 1.f},
            {1.f, 1.f, 1.f}
        };


        auto sun = gameObject::makePointLight(2.0f);
        sun.color = {1.0f, 1.0f, 0.5f}; // yellowish 1 1 0.5
        //auto rotateSun = glm::rotate(glm::mat4(1.f), glm::radians(0.0f), glm::vec3(0.f, 0.f, 1.f));
        sun.transform.translation = glm::vec3(0.f, 0.0f, 3.0f);
        gameObjects.emplace(sun.getId(), std::move(sun));

        auto moon = gameObject::makePointLight(2.0f);
        moon.color = {0.7f, 0.7f, 1.0f}; // blueish 0.7 0.7 1
        //auto rotateMoon = glm::rotate(glm::mat4(1.f), glm::radians(270.0f), glm::vec3(0.f, 0.f, 1.f));
        moon.transform.translation = glm::vec3(0.f, 0.0f, -3.0f); // opposite positions of start for sun and moon
        gameObjects.emplace(moon.getId(), std::move(moon));

        /*for (int i = 0; i < lightColors.size(); i++)
        {
            auto pointLight = gameObject::makePointLight(0.5f);
            pointLight.color = lightColors[i];
            auto rotateLight = glm::rotate(glm::mat4(1.f), (i*glm::two_pi<float>()) / lightColors.size(), glm::vec3(0.f, -1.f, 0.f));
            pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
            gameObjects.emplace(pointLight.getId(), std::move(pointLight));
        }*/
    }

    void app::loadTextures()
    {
        auto textureFilepath = "textures/textures.png";
        textures = engineTexture::createTextureFromFile(device, textureFilepath);
    }


} // namespace engine