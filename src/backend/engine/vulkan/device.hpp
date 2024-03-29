
/*!
* \brief 逻辑设备
*    vulkan操作的主体，大部分操作都基于逻辑设备进行
*  基于手动选定的物理设备，按用途需求进行创建。
*  同时构建 队列和命令池
*/

#ifndef ECAS_BACKEND_VULKAN_DEVICE_HPP_
#define ECAS_BACKEND_VULKAN_DEVICE_HPP_

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>
#include "buffer.hpp"

namespace ecas {
namespace vulkan {

class Device {
public:
    static Device *Create(VkPhysicalDevice physical_device, VkQueueFlags queue_flags,
                          std::vector<const char*> &layers);
    ~Device();

    inline VkDevice device() const { return device_; }
    inline VkCommandPool command_pool() const { return command_pool_; }
    inline VkPhysicalDeviceMemoryProperties &memory_properties() { return memory_properties_; }

    void QueueSubmitAndWait(VkCommandBuffer command_buffer);
    
private:
    Device(VkDevice device, VkCommandPool command_pool, 
           VkPhysicalDevice physical_device, uint32_t queue_family_index);

    // Retrieve the queue family in the physical device as needed 
    // and return the index of the corresponding queue family
    static uint32_t SelectQueueFamily(VkPhysicalDevice physical_device, VkQueueFlags queue_flags);

    VkDevice device_;
    VkCommandPool command_pool_;    
    
    // Get.
    VkPhysicalDeviceMemoryProperties memory_properties_;
    VkQueue queue_;
};

}  // namespace vulkan
}  // namespace ecas

#endif  // ECAS_BACKEND_VULKAN_DEVICE_HPP_
