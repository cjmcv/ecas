
/*!
* \brief 描述符池
*     围绕VkDescriptorPool进行，用于创建描述符集。
*/

#ifndef ECAS_BACKEND_VULKAN_DESCRIPTOR_POOL_HPP_
#define ECAS_BACKEND_VULKAN_DESCRIPTOR_POOL_HPP_

#include <vulkan/vulkan.h>

#include <memory>
#include <unordered_map>
#include <vector>

#include "buffer.hpp"

namespace ecas {
namespace vulkan {

class DescriptorPool {
public:
    // Creates a descriptor pool allowing |max_sets| and maximal number of
    // descriptors for each descriptor type as specified in |descriptor_counts|
    // from |device|.
    static DescriptorPool *Create(
        VkDevice device, uint32_t max_sets,
        std::vector<VkDescriptorPoolSize> &descriptor_counts);

    ~DescriptorPool();

    // Allocates descriptor sets following the given |set_layouts| and returns the
    // mapping from the layout to the concrete set object.
    void AllocateDescriptorSets(std::vector<VkDescriptorSetLayout> &set_layouts);

    inline VkDescriptorSet GetDescriptorSet(VkDescriptorSetLayout layout) { return layout_set_map_[layout]; }
    // 
    void WriteBuffer(VkDescriptorSet set, uint32_t bind, Buffer *buffer, VkDeviceSize offset = 0);

private:
    DescriptorPool(VkDescriptorPool pool, VkDevice device);
    
    VkDevice device_;
    VkDescriptorPool pool_;
    std::unordered_map<VkDescriptorSetLayout, VkDescriptorSet> layout_set_map_;
};

}  // namespace vulkan
}  // namespace uvkc

#endif  // ECAS_BACKEND_VULKAN_DESCRIPTOR_POOL_HPP_
