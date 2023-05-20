/*!
* \brief 着色器模块
*      基于spir-v代码创建VkShaderModule和描述符集layout
*/

#ifndef ECAS_BACKEND_VULKAN_SHADER_MODULE_HPP_
#define ECAS_BACKEND_VULKAN_SHADER_MODULE_HPP_

#include <vulkan/vulkan.h>

#include <memory>
#include <unordered_map>
#include <vector>

namespace ecas {
namespace vulkan {

// 
struct DescriptorSetLayout {    
    VkDescriptorSetLayoutCreateInfo create_info;
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    
    VkDescriptorSetLayout layout;
};

class ShaderModule {
public:
    // Creates a Vulkan shader module from SPIR-V code 
    // and creates descriptor set layout objects
    // for each descriptor set in the shader module.
    static ShaderModule *Create(VkDevice device, 
                                std::vector<VkDescriptorType>& buffer_types,
                                const char* filename);

    static ShaderModule *Create(VkDevice device, 
                                std::vector<VkDescriptorType>& buffer_types,
                                const uint32_t *spirv_data, 
                                size_t spirv_size);

    ~ShaderModule();

    inline VkShaderModule shader_module() const { return shader_module_; }

    inline uint32_t num_sets() const { return set_layouts_.size(); }

    // Returns all descriptor set layout objects for this shader module.
    std::vector<VkDescriptorSetLayout> descriptor_set_layouts() const;

    VkDescriptorSetLayout GetDescriptorSetLayout(uint32_t set) const;

    // Calculates minimal pool size requirements for each descriptor type used in
    // this shader module.
    std::vector<VkDescriptorPoolSize> CalculateDescriptorPoolSize() const;

private:
    ShaderModule(VkShaderModule module, VkDevice device,
                 std::vector<DescriptorSetLayout> set_layouts);
    static uint32_t* ReadSpvFile(uint32_t& length, const char* filename);

private:
    VkDevice device_;

    VkShaderModule shader_module_;

    std::vector<DescriptorSetLayout> set_layouts_;
    // Taken from set_layouts_，used to specify multiple set layouts 
    // when calling vkCreatePipelineLayout
    std::vector<VkDescriptorSetLayout> vk_set_layouts_;
};

} // namespace vulkan
} // namespace ecas

#endif // UVKC_VULKAN_SHADER_MODULE_H_
