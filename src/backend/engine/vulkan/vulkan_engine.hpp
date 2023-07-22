/*!
* \brief VulkanEngine 对外提供vulkan操作的最外层类，
*     而engine/vulkan内其他文件对外部不可见。
*/

#ifndef ECAS_BACKEND_VULKAN_ENGINE_HPP_
#define ECAS_BACKEND_VULKAN_ENGINE_HPP_

#include <vulkan/vulkan.h>
#include "../engine.hpp"

#include "common.hpp"
#include "instance.hpp"
#include "device.hpp"
#include "buffer.hpp"
#include "shader_module.hpp"
#include "pipeline.hpp"
#include "descriptor_pool.hpp"
#include "command_buffer.hpp"

#include "kernel/vulkan/kernel_dispatcher.hpp"

namespace ecas {
namespace vulkan {

class VulkanEngine: public Engine {
    // kernel运行所需资源单元。由Engine创建，一个kernel对应一个ExecUnit
    class ExecUnit {
    public:
        void Run(std::vector<Buffer*> &input_buffers, const int push_constant_size, const void *push_constant, std::vector<Buffer*> &output_buffers);

        KernelParams *params_;
        Device *device_;
        ShaderModule *shader_module_;
        //
        Pipeline *pipeline_;
        DescriptorPool *descriptor_pool_;
        CommandBuffer *command_buffer_;
        //
        VkDescriptorSet descriptor_set_;
    };

public:
    void Init(int physical_device_id = 0, bool enable_validation = false);
    void Deinit();

    Buffer *CreateBuffer(uint32_t size);
    void Run(std::string kernel_name, std::vector<Buffer*> &input_buffers, const int push_constant_size, const void *push_constant, std::vector<Buffer*> &output_buffers);

private:
    void SetKernelMap();

private:
    Instance *instance_;
    Device *device_;

    std::unordered_map<std::string, ExecUnit*> exec_map_;
    VulkanKernelDispatcher *vk_dispatcher_;
};

} // namespace vulkan
} // namespace ecas

#endif // ECAS_BACKEND_VULKAN_ENGINE_HPP_
