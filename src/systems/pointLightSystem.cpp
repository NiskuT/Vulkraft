#include "systems/pointLightSystem.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <cassert>
#include <map>

namespace engine 
{
    struct PointLightPushConstants
    {
        glm::vec4 position;
        glm::vec4 color;
        float radius;
    };

    pointLightSystem::pointLightSystem(engineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
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

        if(vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
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
        p_pipeline = std::make_unique<pipeline>(device, "shaders/point_light.vert.spv", "shaders/point_light.frag.spv", pipelineConfig);
    }

    void pointLightSystem::update(FrameInfo& frameInfo, GlobalUbo& ubo)
    {
        auto rotateLight = glm::rotate(glm::mat4(1.f), frameInfo.frameTime/10, glm::vec3(1.f, -0.f, 0.f)); // turn around x : -1 on z = west, +1 on z = east for the sun, we can change the angle to change the speed of the sun and moon 
        int lightIndex = 0;
        for (auto& kv: frameInfo.gameObjects)
        {
            auto& gameObject = kv.second;
            if (gameObject.pointLight == nullptr) continue;

            assert(lightIndex < MAX_LIGHTS && "Too many lights!");

            // update light position
            gameObject.transform.translation = glm::vec3(rotateLight * glm::vec4(gameObject.transform.translation, 1.0f));

            ubo.pointLights[lightIndex].position = glm::vec4(gameObject.transform.translation, 1.0f);
            ubo.pointLights[lightIndex].color = glm::vec4(gameObject.color, gameObject.pointLight->lightIntensity);
            lightIndex++;
            
        }
        ubo.numLights = lightIndex;
    }

    void pointLightSystem::render(FrameInfo& frameInfo)
    {
        // sort lights by distance to camera
        std::map<float, gameObject::id_t> sorted;
        for (auto& kv: frameInfo.gameObjects)
        {
            auto& gameObject = kv.second;
            if (gameObject.pointLight == nullptr) continue;

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
            auto& gameObject = frameInfo.gameObjects.at(it->second);

            PointLightPushConstants push{};
            push.position = glm::vec4(gameObject.transform.translation, 1.0f);
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

} // namespace engine