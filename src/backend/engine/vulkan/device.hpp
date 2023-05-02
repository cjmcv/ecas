
#ifndef ECAS_BACKEND_VULKAN_DEVICE_HPP_
#define ECAS_BACKEND_VULKAN_DEVICE_HPP_

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace ecas {
namespace vulkan {

// This class representing a Vulkan logical device.
class Device {
public:
    Device(VkPhysicalDevice physical_device, VkQueueFlags queue_flags,
           std::vector<const char*> &layers);
    ~Device();

private:
    uint32_t SelectQueueFamily(VkPhysicalDevice physical_device,
                               VkQueueFlags queue_flags);

// TODO：private
public:
    VkDevice device_;
    
    VkPhysicalDevice physical_device_;
    VkPhysicalDeviceMemoryProperties memory_properties_;
    uint32_t queue_family_index_;
    VkQueue queue_;

};

}  // namespace vulkan
}  // namespace ecas

#endif  // ECAS_BACKEND_VULKAN_DEVICE_HPP_
