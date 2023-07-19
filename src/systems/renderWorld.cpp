#include "systems/renderWorld.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "block.hpp"

#include <stdexcept>
#include <array>

#ifndef SHADER_PATH
  #define SHADER_PATH "shaders/"
#endif

namespace engine 
{

    struct SimplePushConstantData
    {
        glm::mat4 modelMatrix{1.f};
        glm::mat4 normalMatrix{1.f};
    };
    
    renderWorld::renderWorld(engineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
        : device{device}
    {
        createPipelineLayout(globalSetLayout);
        createPipeline(renderPass);
    }

    renderWorld::~renderWorld()
    {
        vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
    }

    void renderWorld::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
    {

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

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

    void renderWorld::createPipeline(VkRenderPass renderPass)
    {
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        pipelineConfigInfo pipelineConfig{};
        pipeline::defaultPipelineConfigInfo(pipelineConfig);
        //pipeline::enableAlphaBlending(pipelineConfig);
        pipelineConfig.attributeDescriptions.clear();
        pipelineConfig.bindingDescriptions.clear();

        pipelineConfig.attributeDescriptions = engine::block::Vertex::getAttributeDescriptions();
        pipelineConfig.bindingDescriptions = engine::block::Vertex::getBindingDescriptions();

        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        p_pipeline = std::make_unique<pipeline>(device, SHADER_PATH "block_shaders.vert.spv", SHADER_PATH "block_shaders.frag.spv", pipelineConfig);
    }


    void renderWorld::renderVisibleWorld(FrameInfo& frameInfo)
    {
        p_pipeline->bind(frameInfo.commandBuffer);

        int playerX = static_cast<int>(frameInfo.myCamera.getPosition().x);
        int playerY = static_cast<int>(frameInfo.myCamera.getPosition().y);

        vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
            0,
            1,
            &frameInfo.globalDescriptorSet,
            0,
            nullptr);

        auto chunkToRender = frameInfo.myWorld.getChunkToRender();
        for (auto& currChunk : chunkToRender)
        {
            SimplePushConstantData push{};
            push.modelMatrix = currChunk->transform.mat4();
            push.normalMatrix = currChunk->transform.normalMatrix();

            vkCmdPushConstants(
                frameInfo.commandBuffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push);
            currChunk->bind(frameInfo.commandBuffer);
            currChunk->draw(frameInfo.commandBuffer);
        }
    }

} // namespace engine