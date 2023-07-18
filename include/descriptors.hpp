#pragma once

#include "engineDevice.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace engine
{

    /*
    First class : engineDescriptorSetLayout
    defines the structure and bindings of the descriptors in a set
    */
    class engineDescriptorSetLayout 
    {
    public:
        class Builder 
        {
            public:
                Builder(engineDevice &device) : device{device} {}

            Builder &addBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<engineDescriptorSetLayout> build() const;

        private:
            engineDevice &device;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        // constructor, destructor, copy
        engineDescriptorSetLayout(
            engineDevice &device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~engineDescriptorSetLayout();
        engineDescriptorSetLayout(const engineDescriptorSetLayout &) = delete;
        engineDescriptorSetLayout &operator=(const engineDescriptorSetLayout &) = delete;
        // retrieve the layout 
        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        engineDevice &device;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class engineDescriptorWriter;
    };

    /*
    Second class : engineDescriptorPool
    manages allocation & deallocation of the descriptors sets 
    */

    class engineDescriptorPool {
        public:
            // Builder class to manage the pool with given properties 
            class Builder {
                public:
                    Builder(engineDevice &device) : device{device} {}

            Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder &setMaxSets(uint32_t count);
            std::unique_ptr<engineDescriptorPool> build() const;

        private:
            engineDevice &device;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        engineDescriptorPool(
            engineDevice &device,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize> &poolSizes);
        ~engineDescriptorPool();
        engineDescriptorPool(const engineDescriptorPool &) = delete;
        engineDescriptorPool &operator=(const engineDescriptorPool &) = delete;

        bool allocateDescriptor(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

        void resetPool();

    private:
        engineDevice &device;
        VkDescriptorPool descriptorPool;

        friend class engineDescriptorWriter;
    };

    /*
    Third class : engineDescriptorWriter
    To write data in a descriptor set, needs the descriptor pool to have all the sets and the layouts of the sets to access it 
    */
    class engineDescriptorWriter {
        public:
            engineDescriptorWriter(engineDescriptorSetLayout &setLayout, engineDescriptorPool &pool);

        engineDescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
        engineDescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

            bool build(VkDescriptorSet &set); // build a new descriptor set
            void overwrite(VkDescriptorSet &set); // overwrite an existing set

    private:
        engineDescriptorSetLayout &setLayout;
        engineDescriptorPool &pool;
        std::vector<VkWriteDescriptorSet> writes;
    };

} // namespace engine