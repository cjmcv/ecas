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

class VulkanEngine: public Engine {
    struct KernelResPack {
        ShaderModule *shader_module;
        std::vector<Pipeline::SpecConstant> spec_constant;
        uint32_t push_constant_num;
        Pipeline *pipeline;
        Buffer *buffer;
        uint32_t buffer_size;
        DescriptorPool *descriptor_pool;
        CommandBuffer *command_buffer;
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

    std::unordered_map<std::string, KernelResPack*> res_map_;
};

} // namespace vulkan
} // namespace ecas

#endif // ECAS_BACKEND_VULKAN_ENGINE_HPP_
