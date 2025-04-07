#pragma once
#include "my_engine_device.h"

// std
#include <memory>
#include <unordered_map>
#include <vector>

class CE_Descriptor_Set_Layout
{
public:
    class Builder {
    public:
        Builder(My_Engine_Device& ce_device) : ce_device{ ce_device } {}

        Builder& add_binding(
            uint32_t binding,
            VkDescriptorType descriptor_type,
            VkShaderStageFlags stage_flags,
            uint32_t count = 1);
        std::unique_ptr<CE_Descriptor_Set_Layout> build() const;

    private:
        My_Engine_Device& ce_device;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
    };

    CE_Descriptor_Set_Layout(My_Engine_Device& ce_device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
    ~CE_Descriptor_Set_Layout();
    CE_Descriptor_Set_Layout(const CE_Descriptor_Set_Layout&) = delete;
    CE_Descriptor_Set_Layout& operator=(const CE_Descriptor_Set_Layout&) = delete;

    VkDescriptorSetLayout get_descriptor_set_layout() const { return descriptor_set_layout; }

private:
    My_Engine_Device& ce_device;
    VkDescriptorSetLayout descriptor_set_layout;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

    friend class CE_Descriptor_Writer;
};

class CE_Descriptor_Pool {
public:
    class Builder {
    public:
        Builder(My_Engine_Device& ce_device) : ce_device{ ce_device } {}

        Builder& add_pool_size(VkDescriptorType descriptor_type, uint32_t count);
        Builder& set_pool_flags(VkDescriptorPoolCreateFlags flags);
        Builder& set_max_sets(uint32_t count);
        std::unique_ptr<CE_Descriptor_Pool> build() const;

    private:
        My_Engine_Device& ce_device;
        std::vector<VkDescriptorPoolSize> pool_sizes{};
        uint32_t max_sets = 1000;
        VkDescriptorPoolCreateFlags pool_flags = 0;
    };

    CE_Descriptor_Pool(
        My_Engine_Device& ce_device,
        uint32_t max_sets,
        VkDescriptorPoolCreateFlags pool_flags,
        const std::vector<VkDescriptorPoolSize>& pool_sizes);
    ~CE_Descriptor_Pool();
    CE_Descriptor_Pool(const CE_Descriptor_Pool&) = delete;
    CE_Descriptor_Pool& operator=(const CE_Descriptor_Pool&) = delete;

    bool allocate_descriptor(
        const VkDescriptorSetLayout descriptor_set_layout, VkDescriptorSet& descriptor) const;

    void free_descriptors(std::vector<VkDescriptorSet>& descriptors) const;

    void reset_pool();

private:
    My_Engine_Device& ce_device;
    VkDescriptorPool descriptor_pool;

    friend class CE_Descriptor_Writer;
};

class CE_Descriptor_Writer {
public:
    CE_Descriptor_Writer(CE_Descriptor_Set_Layout& set_layout, CE_Descriptor_Pool& pool);

    CE_Descriptor_Writer& write_buffer(uint32_t binding, VkDescriptorBufferInfo* buffer_info);
    CE_Descriptor_Writer& write_image(uint32_t binding, VkDescriptorImageInfo* imageInfo);

    bool build(VkDescriptorSet& set);
    void overwrite(VkDescriptorSet& set);

private:
    CE_Descriptor_Set_Layout& set_layout;
    CE_Descriptor_Pool& pool;
    std::vector<VkWriteDescriptorSet> writes;
};

