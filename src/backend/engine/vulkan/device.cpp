

#include "device.hpp"

#include "util/logger.hpp"
#include "common.hpp"

namespace ecas {
namespace vulkan {

// Selects a queue family with the required |queue_flags| in |physical_device|
// If found, writes the |valid_timestamp_bits| and returns the queue family
// index.
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
}

Device::~Device() {
    vkDestroyDevice(device_, NULL);
}

}  // namespace vulkan
}  // namespace ecas
