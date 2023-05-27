/*!
* \brief Buffer
*    管理Buffer和与之绑定的memory
*/

#ifndef ECAS_BACKEND_VULKAN_BUFFER_HPP_
#define ECAS_BACKEND_VULKAN_BUFFER_HPP_

#include <vulkan/vulkan.h>

namespace ecas {
namespace vulkan {
  
class Buffer {
public:
    // Create VkBuffer, Allocate VkDeviceMemory, and bind the buffer and memory
    static Buffer* Create(VkDevice device, VkPhysicalDeviceMemoryProperties &memory_properties,
                          VkBufferUsageFlags usage_flags, 
                          VkMemoryPropertyFlags memory_flags,
                          VkDeviceSize size);

    ~Buffer();

    inline VkBuffer buffer() const { return buffer_; }
    inline uint32_t buffer_size() const { return buffer_size_; }

    // Gets a CPU accessible memory address for the current buffer.
    void *MapMemory(size_t offset, size_t size);

    // Invalidate the CPU accessible memory address for the current buffer.
    void UnmapMemory();

private:
    Buffer(VkDevice device, VkBuffer buffer, VkDeviceSize size, VkDeviceMemory memory);
    static uint32_t SelectMemoryType(VkPhysicalDeviceMemoryProperties &memory_properties,
                                     uint32_t supported_memory_types,
                                     VkMemoryPropertyFlags desired_memory_properties);
    static VkDeviceMemory AllocateMemory(VkDevice device, 
                                         VkPhysicalDeviceMemoryProperties &memory_properties,
                                         VkMemoryRequirements memory_requirements,
                                         VkMemoryPropertyFlags memory_flags);

private: 
    VkDevice device_;

    VkBuffer buffer_;
    uint32_t buffer_size_;
    VkDeviceMemory memory_;
};

}  // end of namespace vulkan
}  // end of namespace ecas

#endif  // ECAS_BACKEND_VULKAN_BUFFER_HPP_
