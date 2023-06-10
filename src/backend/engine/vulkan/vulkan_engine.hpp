/*!
* \brief VulkanEngine vulkan操作最外层api
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

namespace ecas {
namespace vulkan {

// 固定参数，在kernel注册时指定
struct KernelParams {
    std::vector<Pipeline::SpecConstant> spec_constant;
    uint32_t push_constant_num;
    uint32_t workgroup_size[3];
};

class VulkanEngine: public Engine {
    // kernel运行所需资源单元。由Engine创建，一个kernel对应一个executor
    class ExecUnit {
    public:
        void Run();

        KernelParams *params;
        Device *device_;
        ShaderModule *shader_module_;
        //
        Pipeline *pipeline_;
        Buffer *buffer_;
        uint32_t buffer_size_;
        DescriptorPool *descriptor_pool_;
        CommandBuffer *command_buffer_;
        //
        VkDescriptorSet descriptor_set_;
    };

public:
    void Init(int physical_device_id = 0, bool enable_validation = false);
    void Deinit();
    void Run(std::string kernel_name);

private:
    void SetKernelMap();

private:
    Instance *instance_;
    Device *device_;

    std::unordered_map<std::string, ExecUnit*> exec_map_;
};

} // namespace vulkan
} // namespace ecas

#endif // ECAS_BACKEND_VULKAN_ENGINE_HPP_
