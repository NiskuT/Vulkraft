#include "systems/pointLightSystem.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <cassert>
#include <map>

#include <iostream>

#ifndef SHADER_PATH
#define SHADER_PATH "shaders/"
#endif

namespace engine
{
    struct PointLightPushConstants
    {
        glm::vec4 position;
        glm::vec4 color;
        float radius;
    };

    pointLightSystem::pointLightSystem(engineDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
        : device{device}
    {
        createPipelineLayout(globalSetLayout);
        createPipeline(renderPass);
    }

    pointLightSystem::~pointLightSystem()
    {
        vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
    }

    void pointLightSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
    {

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(PointLightPushConstants);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void pointLightSystem::createPipeline(VkRenderPass renderPass)
    {
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        pipelineConfigInfo pipelineConfig{};
        pipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipeline::enableAlphaBlending(pipelineConfig);
        pipelineConfig.attributeDescriptions.clear();
        pipelineConfig.bindingDescriptions.clear();

        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        p_pipeline = std::make_unique<pipeline>(device, SHADER_PATH "point_light.vert.spv", SHADER_PATH "point_light.frag.spv", pipelineConfig);
    }

    void pointLightSystem::update(FrameInfo &frameInfo, GlobalUbo &ubo)
    {
        auto rotateLight = glm::rotate(glm::mat4(1.f), frameInfo.frameTime / 10, glm::vec3(1.f, 0.f, 0.f));
        // turn around x : -1 on z = west, +1 on z = east for the sun, we can change the angle to change the speed of the sun and moon
        int lightIndex = 0;
        for (auto &kv : frameInfo.gameObjects)
        {
            assert(lightIndex < MAX_LIGHTS && "Too many lights!");

            auto &gameObject = kv.second;
            if (gameObject.pointLight == nullptr)
                continue;
            if (lightIndex <= 1)
            {
                // update psotion of the sun and moon
                gameObject.transform.translation = glm::vec3(rotateLight * glm::vec4(gameObject.transform.translation, 1.0f));

                ubo.pointLights[lightIndex].position = glm::vec4(gameObject.transform.translation, 1.0f);
                ubo.pointLights[lightIndex].color = glm::vec4(gameObject.color, gameObject.pointLight->lightIntensity);
                lightIndex++;
            }
        }
        ubo.numLights = lightIndex;
    }

    float pointLightSystem::getSunHeight(FrameInfo &frameInfo)
    {
        for (auto &kv : frameInfo.gameObjects)
        {
            auto &gameObject = kv.second;
            if (gameObject.pointLight == nullptr)
                continue;
            float height = gameObject.transform.translation.y;
            return height/MAX_SUN_HEIGHT;
        }
        return 0;
    }

    void pointLightSystem::render(FrameInfo &frameInfo)
    {
        // sort lights by distance to camera
        std::map<float, gameObject::id_t> sorted;
        for (auto &kv : frameInfo.gameObjects)
        {
            auto &gameObject = kv.second;
            if (gameObject.pointLight == nullptr)
                continue;

            // calculate distance to camera
            auto offset = frameInfo.myCamera.getPosition() - gameObject.transform.translation;
            float disSquared = glm::dot(offset, offset);
            sorted[disSquared] = gameObject.getId();
        }

        p_pipeline->bind(frameInfo.commandBuffer);

        vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
            0,
            1,
            &frameInfo.globalDescriptorSet,
            0,
            nullptr);

        // iterate through sorted lights in reverse order
        for (auto it = sorted.rbegin(); it != sorted.rend(); ++it)
        {
            // use game object id to find the light object
            auto &gameObject = frameInfo.gameObjects.at(it->second);

            PointLightPushConstants push{};
            push.position = glm::vec4(gameObject.transform.translation, 1.0f);
            push.position.x += frameInfo.myCamera.getPosition().x;
            push.position.z += frameInfo.myCamera.getPosition().z;
            push.color = glm::vec4(gameObject.color, gameObject.pointLight->lightIntensity);
            push.radius = gameObject.transform.scale.x;

            vkCmdPushConstants(
                frameInfo.commandBuffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(PointLightPushConstants),
                &push);

            vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
        }
    }

    glm::vec3 pointLightSystem::getSkyColor(float sunHeight) {

    glm::vec3 twilightColor = glm::vec3(1.0f, 0.5f, 0.0f); // orange for the sunset
    glm::vec3 duskColor = glm::vec3(0.0f, 0.0f, 0.5f); // blue for the night just after the sunset
    glm::vec3 dayColor = glm::vec3(0.5f, 0.7f, 1.0f); // blue for the day
    glm::vec3 nightColor = glm::vec3(0.0f, 0.0f, 0.0f); //  black for the night

    float twilightHeight = -0.2f;
    float duskHeight = -0.4f;
    float dayHeight = 0.2f;

    // Linear interpolation between colors depending on the height of the sun
    if (sunHeight > dayHeight) {
        return dayColor;
    } else if (sunHeight > 0) {
        // Sunset
        float t = (sunHeight - dayHeight) / (.0f - dayHeight);
        return glm::mix(dayColor, twilightColor, t);
    } else if (sunHeight > twilightHeight) {
        // After sunset
        float t = (sunHeight - twilightHeight) / (0.f - twilightHeight);
        return glm::mix(duskColor, twilightColor, t);
    } else if (sunHeight > duskHeight) {
        // Transition to night
        float t = (sunHeight - duskHeight) / (twilightHeight - duskHeight);
        return glm::mix(nightColor, duskColor, t);
    } else {
        // Night
        return nightColor;
    }
}

} // namespace engine