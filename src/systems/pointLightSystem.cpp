#include "systems/pointLightSystem.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>

namespace engine 
{
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

        /*VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);*/

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

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
        pipelineConfig.attributeDescriptions.clear();
        pipelineConfig.bindingDescriptions.clear();

        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        p_pipeline = std::make_unique<pipeline>(device, "shaders/point_light.vert.spv", "shaders/point_light.frag.spv", pipelineConfig);
    }

    void pointLightSystem::render(FrameInfo& frameInfo)
    {
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

        vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
    }

} // namespace engine