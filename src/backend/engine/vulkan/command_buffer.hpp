/*!
* \brief 命令缓冲区
*     围绕VkCommandBuffer进行，由命令池创建，在命令池释放时释放
*     主要用于命令的记录
*/

#ifndef ECAS_BACKEND_VULKAN_COMMAND_BUFFER_HPP_
#define ECAS_BACKEND_VULKAN_COMMAND_BUFFER_HPP_

#include <vulkan/vulkan.h>

#include <memory>
#include <unordered_map>

#include "buffer.hpp"
#include "pipeline.hpp"

namespace ecas {
namespace vulkan {

class CommandBuffer {
public:
    static CommandBuffer *Create(VkDevice device, VkCommandPool command_pool);
    ~CommandBuffer();

    inline VkCommandBuffer command_buffer() const { return command_buffer_; };

    // Begins command buffer recording.
    void Begin();
    // Ends command buffer recording.
    void End();

    // Records a command to copy the src_buffer to dst_buffer.
    void CopyBuffer(const Buffer &src_buffer, size_t src_offset,
                    const Buffer &dst_buffer, size_t dst_offset, size_t length);

    // Records a command to bind the compute |pipeline| and resource descriptor
    // sets recorded in |bound_descriptor_sets| into this command buffer.
    void BindPipelineAndDescriptorSets(
        const Pipeline *pipeline,
        std::vector<VkDescriptorSet> &&descriptor_sets);

    void PushConstant(VkPipelineLayout layout, const int params_size, const void *params);

    // Records a dispatch command.
    void Dispatch(uint32_t x, uint32_t y, uint32_t z);

    // Records a pipeline barrier that synchronizes shader read from a compute
    // shader with shader write from a previous compute shader.
    void DispatchBarrier();

private:
    CommandBuffer(VkDevice device, VkCommandBuffer command_buffer);

    VkDevice device_;
    VkCommandBuffer command_buffer_;
};

}  // namespace vulkan
}  // namespace ecas

#endif  // ECAS_BACKEND_VULKAN_COMMAND_BUFFER_HPP_
