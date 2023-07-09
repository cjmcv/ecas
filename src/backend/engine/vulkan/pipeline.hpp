/*!
* \brief 计算管线
*      围绕VkPipeline进行，
*/

#ifndef ECAS_BACKEND_VULKAN_PIPELINE_HPP_
#define ECAS_BACKEND_VULKAN_PIPELINE_HPP_

#include <vulkan/vulkan.h>
#include "shader_module.hpp"

namespace ecas {
namespace vulkan {

struct SpecConstant {
    uint32_t id;
    union {
        int32_t s32;
        uint32_t u32;
        float f32;
    } value;
};

class Pipeline {
public:

    union PushConstant {
        int i;
        float f;
    };
    // Creates a Vulkan compute pipeline
    static Pipeline *Create(VkDevice device, 
                           VkShaderModule shader_module, 
                           std::vector<VkDescriptorSetLayout> &set_layouts,
                           const char *entry_point, 
                           std::vector<SpecConstant> &spec_constants,
                           uint32_t push_constant_count);

    ~Pipeline();

    inline VkPipeline pipeline() const { return pipeline_; }
    inline VkPipelineLayout pipeline_layout() const { return pipeline_layout_; }

private:
    Pipeline(VkPipeline pipeline, VkDevice device, VkPipelineLayout layout);
    
    VkDevice device_;
    
    VkPipeline pipeline_;
    VkPipelineLayout pipeline_layout_;
};

}  // end of namespace vulkan
}  // end of namespace ecas

#endif  // ECAS_BACKEND_VULKAN_PIPELINE_HPP_
