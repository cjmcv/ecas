

#include "device.hpp"

#include "util/logger.hpp"
#include "common.hpp"

namespace ecas {
namespace vulkan {

// 按需检索物理设备中的队列簇，并返回对应队列簇在该物理设备上的id号
uint32_t Device::SelectQueueFamily(VkPhysicalDevice physical_device,
                                   VkQueueFlags queue_flags) {
    uint32_t count;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &count, nullptr);

    std::vector<VkQueueFamilyProperties> queue_families(count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &count, queue_families.data());

    for (int index = 0; index < count; ++index) {
        const VkQueueFamilyProperties &properties = queue_families[index];
        if (properties.queueCount > 0 &&
            ((properties.queueFlags & queue_flags) == queue_flags)) {
            return index;
        }
    }

    ECAS_LOGE("Instance::SelectQueueFamily -> Cannot find queue family with required bits.");
    return 0;
}

void Device::QueueSubmitAndWait(VkCommandBuffer command_buffer) {
    VkFenceCreateInfo fence_create_info = {};
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_create_info.pNext = nullptr;
    fence_create_info.flags = 0;

    VkFence fence = VK_NULL_HANDLE;
    vkCreateFence(device_, &fence_create_info, /*pALlocator=*/nullptr, &fence);

    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;

    vkQueueSubmit(queue_, 1, &submit_info, fence);

    vkWaitForFences(device_, /*fenceCount=*/1, &fence,
                                                /*waitAll=*/true,
                                                /*timeout=*/UINT64_MAX);

    vkDestroyFence(device_, fence, /*pAllocator=*/nullptr);
}

Device::Device(VkPhysicalDevice physical_device, VkQueueFlags queue_flags, 
               std::vector<const char*> &layers) {

    physical_device_ = physical_device;
    queue_family_index_ = SelectQueueFamily(physical_device_, queue_flags);

    float queue_priority = 1.0;

    VkDeviceQueueCreateInfo queue_create_info = {};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.pNext = nullptr;
    queue_create_info.flags = 0;
    queue_create_info.queueFamilyIndex = queue_family_index_;
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &queue_priority;

    VkDeviceCreateInfo device_create_info = {};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.pNext = nullptr;
    device_create_info.flags = 0;
    device_create_info.queueCreateInfoCount = 1;
    device_create_info.pQueueCreateInfos = &queue_create_info;
    device_create_info.enabledLayerCount = layers.size();
    device_create_info.ppEnabledLayerNames = layers.data();
    // device_create_info.enabledExtensionCount = extensions.size();
    // device_create_info.ppEnabledExtensionNames = extensions.data();
    device_create_info.pEnabledFeatures = nullptr;

    vkCreateDevice(physical_device_, &device_create_info, /*pAllocator=*/nullptr, &device_);

    vkGetPhysicalDeviceMemoryProperties(physical_device_, &memory_properties_);
    // Get a handle to the only member of the queue family.
    vkGetDeviceQueue(device_, queue_family_index_, 0, &queue_);

    // Create Command pool
    VkCommandPoolCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    create_info.queueFamilyIndex = queue_family_index_;
    vkCreateCommandPool(device_, &create_info, /*pAllocator=*/nullptr, &command_pool_);
}

Device::~Device() {
    vkDestroyCommandPool(device_, command_pool_, NULL);	
    vkDestroyDevice(device_, NULL);
}

}  // namespace vulkan
}  // namespace ecas
