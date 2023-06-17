#include "buffer.hpp"
#include "common.hpp"
#include "util/logger.hpp"

namespace ecas {
namespace vulkan {

uint32_t Buffer::SelectMemoryType(VkPhysicalDeviceMemoryProperties &memory_properties,
                                  uint32_t supported_memory_types,
                                  VkMemoryPropertyFlags desired_memory_properties) {
    for (int i = 0; i < memory_properties.memoryTypeCount; ++i) {
        if ((supported_memory_types & (1 << i)) &&
            ((memory_properties.memoryTypes[i].propertyFlags &
            desired_memory_properties) == desired_memory_properties))
        return i;
    }
    ECAS_LOGE("Cannot find memory type with required bits.\n");
    return -1;
}

VkDeviceMemory Buffer::AllocateMemory(VkDevice device, 
                                      VkPhysicalDeviceMemoryProperties &memory_properties,
                                      VkMemoryRequirements memory_requirements,
                                      VkMemoryPropertyFlags memory_flags) {
    VkMemoryAllocateInfo allocate_info = {};
    allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocate_info.pNext = nullptr;
    allocate_info.allocationSize = memory_requirements.size;
    allocate_info.memoryTypeIndex = SelectMemoryType(memory_properties, memory_requirements.memoryTypeBits, memory_flags);

    VkDeviceMemory memory = VK_NULL_HANDLE;
    vkAllocateMemory(device, &allocate_info, /*pAlloator=*/nullptr, &memory);
    return memory;
}

Buffer *Buffer::Create(VkDevice device, 
                       VkPhysicalDeviceMemoryProperties &memory_properties, 
                       VkBufferUsageFlags usage_flags, 
                       VkMemoryPropertyFlags memory_flags,
                       VkDeviceSize size) {

    VkBufferCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.size = size;
    create_info.usage = usage_flags;
    create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkBuffer buffer;
    vkCreateBuffer(device, &create_info, /*pAllocator=*/nullptr, &buffer);
    
    // Get memory requirements for the buffer
    VkMemoryRequirements memory_requirements;
    vkGetBufferMemoryRequirements(device, buffer, &memory_requirements);

    // Allocate memory for the buffer
    VkDeviceMemory memory = AllocateMemory(device, memory_properties, memory_requirements, memory_flags);

    // Bind the memory to the buffer
    vkBindBufferMemory(device, buffer, memory, /*memoryOffset=*/0);

    return new Buffer(device, buffer, size, memory);
}

Buffer::Buffer(VkDevice device, VkBuffer buffer, VkDeviceSize size, VkDeviceMemory memory) 
    : device_(device), buffer_(buffer), buffer_size_(size), memory_(memory) {}

Buffer::~Buffer() {
    vkDestroyBuffer(device_, buffer_, /*pAllocator=*/nullptr);
    vkFreeMemory(device_, memory_, /*pAllocator=*/nullptr);
}

void *Buffer::MapMemory(size_t offset, size_t size) {
    void *data = nullptr;
    VK_CHECK(vkMapMemory(device_, memory_, offset, size, /*flags=*/0, &data));
    return data;
}

void Buffer::UnmapMemory() { vkUnmapMemory(device_, memory_); }

}  // namespace vulkan
}  // namespace ecas
