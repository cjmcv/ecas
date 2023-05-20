

#include "command_buffer.hpp"

namespace ecas {
namespace vulkan {

CommandBuffer *CommandBuffer::Create(VkDevice device, VkCommandPool command_pool) {
    VkCommandBuffer command_buffer;
    
    // Now allocate a command buffer from the command pool.
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = command_pool; // specify the command pool to allocate from.
    // if the command buffer is primary, it can be directly submitted to queues.
    // A secondary buffer has to be called from some primary command buffer, and cannot be directly
    // submitted to a queue. To keep things simple, we use a primary command buffer.
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;                                       // allocate a single command buffer.
    vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &command_buffer); // allocate command buffer.

    return new CommandBuffer(device, command_buffer);
}

CommandBuffer::CommandBuffer(VkDevice device, VkCommandBuffer command_buffer)
    : command_buffer_(command_buffer), device_(device) {}

CommandBuffer::~CommandBuffer() = default;

void CommandBuffer::Begin() {
    vkResetCommandBuffer(command_buffer_, /*flags=*/0);

    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.pNext = nullptr;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    begin_info.pInheritanceInfo = nullptr;
    vkBeginCommandBuffer(command_buffer_, &begin_info);
}

void CommandBuffer::End() {
    vkEndCommandBuffer(command_buffer_);
}

void CommandBuffer::CopyBuffer(const Buffer &src_buffer, size_t src_offset,
                               const Buffer &dst_buffer, size_t dst_offset,
                               size_t length) {
    VkBufferCopy region = {};
    region.srcOffset = src_offset;
    region.dstOffset = dst_offset;
    region.size = length;
    vkCmdCopyBuffer(command_buffer_, src_buffer.buffer(),
                            dst_buffer.buffer(),
                            /*regionCount=*/1, &region);
}

void CommandBuffer::BindPipelineAndDescriptorSets(
    const Pipeline *pipeline,
    std::vector<VkDescriptorSet> &&descriptor_sets) {

    vkCmdBindPipeline(command_buffer_, VK_PIPELINE_BIND_POINT_COMPUTE,
                             pipeline->pipeline());

    for (int i=0; i<descriptor_sets.size(); i++) {
        vkCmdBindDescriptorSets(
            command_buffer_, VK_PIPELINE_BIND_POINT_COMPUTE,
            pipeline->pipeline_layout(), 
            /*firstSet=*/i,
            /*descriptorSetCount=*/1,
            /*pDescriptorSets=*/&descriptor_sets[i],
            /*dynamicOffsetCount=*/0,
            /*pDynamicOffsets=*/nullptr);
    }
}

void CommandBuffer::Dispatch(uint32_t x, uint32_t y, uint32_t z) {
    vkCmdDispatch(command_buffer_, x, y, z);
}

void CommandBuffer::DispatchBarrier() {
    VkMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(command_buffer_,
                         VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                         VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 1,
                         &barrier, 0, nullptr, 0, nullptr);
}

}  // namespace vulkan
}  // namespace uvkc
