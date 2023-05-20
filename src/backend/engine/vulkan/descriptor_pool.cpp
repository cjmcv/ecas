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

std::unordered_map<VkDescriptorSetLayout, VkDescriptorSet>
DescriptorPool::AllocateDescriptorSets(std::vector<VkDescriptorSetLayout> &set_layouts) {
    VkDescriptorSetAllocateInfo allocate_info = {};
    allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocate_info.pNext = nullptr;
    allocate_info.descriptorPool = pool_;
    allocate_info.descriptorSetCount = set_layouts.size();
    allocate_info.pSetLayouts = set_layouts.data();

    std::vector<VkDescriptorSet> sets(set_layouts.size());
    vkAllocateDescriptorSets(device_, &allocate_info, sets.data());
    std::unordered_map<VkDescriptorSetLayout, VkDescriptorSet> layout_set_map;
    for (int i = 0; i < set_layouts.size(); ++i) {
        layout_set_map[set_layouts[i]] = sets[i];
    }

    return layout_set_map;
}

DescriptorPool::DescriptorPool(VkDescriptorPool pool, VkDevice device)
    : pool_(pool), device_(device) {}

}  // namespace vulkan
}  // namespace uvkc
