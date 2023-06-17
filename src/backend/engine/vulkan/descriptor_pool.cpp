#include "descriptor_pool.hpp"

#include <vector>

namespace ecas {
namespace vulkan {

DescriptorPool *DescriptorPool::Create(VkDevice device, uint32_t max_sets,
                                       std::vector<VkDescriptorPoolSize> &descriptor_counts) {

    VkDescriptorPoolCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.maxSets = max_sets;
    create_info.poolSizeCount = descriptor_counts.size();
    create_info.pPoolSizes = descriptor_counts.data();

    VkDescriptorPool pool = VK_NULL_HANDLE;
    vkCreateDescriptorPool(device, &create_info, /*pAllocator=*/nullptr, &pool);

    return new DescriptorPool(pool, device);
}

DescriptorPool::~DescriptorPool() {
    vkDestroyDescriptorPool(device_, pool_, /*pALlocator=*/nullptr);
}

void DescriptorPool::AllocateDescriptorSets(std::vector<VkDescriptorSetLayout> &set_layouts) {
    VkDescriptorSetAllocateInfo allocate_info = {};
    allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocate_info.pNext = nullptr;
    allocate_info.descriptorPool = pool_;
    allocate_info.descriptorSetCount = set_layouts.size();
    allocate_info.pSetLayouts = set_layouts.data();

    std::vector<VkDescriptorSet> sets(set_layouts.size());
    vkAllocateDescriptorSets(device_, &allocate_info, sets.data());
    for (int i = 0; i < set_layouts.size(); ++i) {
        layout_set_map_[set_layouts[i]] = sets[i];
    }
}

void DescriptorPool::WriteBuffer(VkDescriptorSet descriptor_set, uint32_t bind, Buffer *buffer, VkDeviceSize offset) {
    // Specify the buffer to bind to the descriptor.
    VkDescriptorBufferInfo descriptor_buffer_info = {};
    descriptor_buffer_info.buffer = buffer->buffer();
    descriptor_buffer_info.offset = offset;
    descriptor_buffer_info.range = buffer->buffer_size();

    VkWriteDescriptorSet write_descriptor_set = {};
    write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptor_set.dstSet = descriptor_set;                             // write to this descriptor set.
    write_descriptor_set.dstBinding = bind;                                     // write to the first, and only binding.
    write_descriptor_set.descriptorCount = 1;                                // update a single descriptor.
    write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; // storage buffer.
    write_descriptor_set.pBufferInfo = &descriptor_buffer_info;

    // perform the update of the descriptor set.
    vkUpdateDescriptorSets(device_, 1, &write_descriptor_set, 0, NULL);
}

DescriptorPool::DescriptorPool(VkDescriptorPool pool, VkDevice device)
    : pool_(pool), device_(device) {}

}  // namespace vulkan
}  // namespace ecas
