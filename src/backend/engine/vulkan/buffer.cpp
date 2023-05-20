#include "buffer.hpp"
#include "common.hpp"

namespace ecas {
namespace vulkan {

uint32_t Buffer::SelectMemoryType(uint32_t supported_memory_types,
                                  VkMemoryPropertyFlags desired_memory_properties) {
    for (int i = 0; i < memory_properties_.memoryTypeCount; ++i) {
        if ((supported_memory_types & (1 << i)) &&
            ((memory_properties_.memoryTypes[i].propertyFlags &
            desired_memory_properties) == desired_memory_properties))
        return i;
    }
    // return absl::UnavailableError("cannot find memory type with required bits");
    return -1;
}

VkDeviceMemory Buffer::AllocateMemory(VkMemoryRequirements memory_requirements,
                                      VkMemoryPropertyFlags memory_flags) {
    VkMemoryAllocateInfo allocate_info = {};
    allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocate_info.pNext = nullptr;
    allocate_info.allocationSize = memory_requirements.size;
    allocate_info.memoryTypeIndex = SelectMemoryType(memory_requirements.memoryTypeBits, memory_flags);

    VkDeviceMemory memory = VK_NULL_HANDLE;
    vkAllocateMemory(device_, &allocate_info, /*pAlloator=*/nullptr, &memory);
    return memory;
}

Buffer::Buffer(VkDevice device, 
               VkPhysicalDeviceMemoryProperties memory_properties, 
               VkBufferUsageFlags usage_flags, 
               VkMemoryPropertyFlags memory_flags,
               VkDeviceSize size)
    : device_(device), memory_properties_(memory_properties)  {

    VkBufferCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.size = size;
    create_info.usage = usage_flags;
    create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    vkCreateBuffer(device_, &create_info, /*pAllocator=*/nullptr, &buffer_);
    buffer_size_ = size;
    
    // Get memory requirements for the buffer
    VkMemoryRequirements memory_requirements;
    vkGetBufferMemoryRequirements(device_, buffer_, &memory_requirements);

    // Allocate memory for the buffer
    memory_ = AllocateMemory(memory_requirements, memory_flags);

    // Bind the memory to the buffer
    vkBindBufferMemory(device_, buffer_, memory_, /*memoryOffset=*/0);
}

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
}  // namespace uvkc
