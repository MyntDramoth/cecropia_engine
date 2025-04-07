#include "ce_descriptor_pool.h"
// std
#include <cassert>
#include <stdexcept>

// *************** Descriptor Set Layout Builder *********************

CE_Descriptor_Set_Layout::Builder& CE_Descriptor_Set_Layout::Builder::add_binding(
    uint32_t binding,
    VkDescriptorType descriptor_type,
    VkShaderStageFlags stage_flags,
    uint32_t count) {
    assert(bindings.count(binding) == 0 && "Binding already in use");
    VkDescriptorSetLayoutBinding layout_binding{};
    layout_binding.binding = binding;
    layout_binding.descriptorType = descriptor_type;
    layout_binding.descriptorCount = count;
    layout_binding.stageFlags = stage_flags;
    bindings[binding] = layout_binding;
    return *this;
}

std::unique_ptr<CE_Descriptor_Set_Layout> CE_Descriptor_Set_Layout::Builder::build() const {
    return std::make_unique<CE_Descriptor_Set_Layout>(ce_device, bindings);
}

// *************** Descriptor Set Layout *********************

CE_Descriptor_Set_Layout::CE_Descriptor_Set_Layout(
    My_Engine_Device& ce_device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
    : ce_device{ ce_device }, bindings{ bindings } {
    std::vector<VkDescriptorSetLayoutBinding> set_layout_bindings{};
    for (auto kv : bindings) {
        set_layout_bindings.push_back(kv.second);
    }

    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info{};
    descriptor_set_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptor_set_layout_info.bindingCount = static_cast<uint32_t>(set_layout_bindings.size());
    descriptor_set_layout_info.pBindings = set_layout_bindings.data();

    if (vkCreateDescriptorSetLayout(
        ce_device.device(),
        &descriptor_set_layout_info,
        nullptr,
        &descriptor_set_layout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

CE_Descriptor_Set_Layout::~CE_Descriptor_Set_Layout() {
    vkDestroyDescriptorSetLayout(ce_device.device(), descriptor_set_layout, nullptr);
}

// *************** Descriptor Pool Builder *********************

CE_Descriptor_Pool::Builder& CE_Descriptor_Pool::Builder::add_pool_size(
    VkDescriptorType descriptor_type, uint32_t count) {
    pool_sizes.push_back({ descriptor_type, count });
    return *this;
}

CE_Descriptor_Pool::Builder& CE_Descriptor_Pool::Builder::set_pool_flags(
    VkDescriptorPoolCreateFlags flags) {
    pool_flags = flags;
    return *this;
}
CE_Descriptor_Pool::Builder& CE_Descriptor_Pool::Builder::set_max_sets(uint32_t count) {
    max_sets = count;
    return *this;
}

std::unique_ptr<CE_Descriptor_Pool> CE_Descriptor_Pool::Builder::build() const {
    return std::make_unique<CE_Descriptor_Pool>(ce_device, max_sets, pool_flags, pool_sizes);
}

// *************** Descriptor Pool *********************

CE_Descriptor_Pool::CE_Descriptor_Pool(
    My_Engine_Device& ce_device,
    uint32_t max_sets,
    VkDescriptorPoolCreateFlags pool_flags,
    const std::vector<VkDescriptorPoolSize>& pool_sizes)
    : ce_device{ ce_device } {
    VkDescriptorPoolCreateInfo descriptor_pool_info{};
    descriptor_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptor_pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    descriptor_pool_info.pPoolSizes = pool_sizes.data();
    descriptor_pool_info.maxSets = max_sets;
    descriptor_pool_info.flags = pool_flags;

    if (vkCreateDescriptorPool(ce_device.device(), &descriptor_pool_info, nullptr, &descriptor_pool) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

CE_Descriptor_Pool::~CE_Descriptor_Pool() {
    vkDestroyDescriptorPool(ce_device.device(), descriptor_pool, nullptr);
}

bool CE_Descriptor_Pool::allocate_descriptor(
    const VkDescriptorSetLayout descriptor_set_layout, VkDescriptorSet& descriptor) const {
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptor_pool;
    allocInfo.pSetLayouts = &descriptor_set_layout;
    allocInfo.descriptorSetCount = 1;

    // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
    // a new pool whenever an old pool fills up. But this is beyond our current scope
    if (vkAllocateDescriptorSets(ce_device.device(), &allocInfo, &descriptor) != VK_SUCCESS) {
        return false;
    }
    return true;
}

void CE_Descriptor_Pool::free_descriptors(std::vector<VkDescriptorSet>& descriptors) const {
    vkFreeDescriptorSets(
        ce_device.device(),
        descriptor_pool,
        static_cast<uint32_t>(descriptors.size()),
        descriptors.data());
}

void CE_Descriptor_Pool::reset_pool() {
    vkResetDescriptorPool(ce_device.device(), descriptor_pool, 0);
}

// *************** Descriptor Writer *********************

CE_Descriptor_Writer::CE_Descriptor_Writer(CE_Descriptor_Set_Layout& set_layout, CE_Descriptor_Pool& pool)
    : set_layout{ set_layout }, pool{ pool } {}

CE_Descriptor_Writer& CE_Descriptor_Writer::write_buffer(
    uint32_t binding, VkDescriptorBufferInfo* buffer_info) {
    assert(set_layout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

    auto& bindingDescription = set_layout.bindings[binding];

    assert(
        bindingDescription.descriptorCount == 1 &&
        "Binding single descriptor info, but binding expects multiple");

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pBufferInfo = buffer_info;
    write.descriptorCount = 1;

    writes.push_back(write);
    return *this;
}

CE_Descriptor_Writer& CE_Descriptor_Writer::write_image(
    uint32_t binding, VkDescriptorImageInfo* image_info) {
    assert(set_layout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

    auto& bindingDescription = set_layout.bindings[binding];

    assert(
        bindingDescription.descriptorCount == 1 &&
        "Binding single descriptor info, but binding expects multiple");

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pImageInfo = image_info;
    write.descriptorCount = 1;

    writes.push_back(write);
    return *this;
}

bool CE_Descriptor_Writer::build(VkDescriptorSet& set) {
    bool success = pool.allocate_descriptor(set_layout.get_descriptor_set_layout(), set);
    if (!success) {
        return false;
    }
    overwrite(set);
    return true;
}

void CE_Descriptor_Writer::overwrite(VkDescriptorSet& set) {
    for (auto& write : writes) {
        write.dstSet = set;
    }
    vkUpdateDescriptorSets(pool.ce_device.device(), writes.size(), writes.data(), 0, nullptr);
}