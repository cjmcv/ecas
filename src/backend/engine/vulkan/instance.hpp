
/*!
* \brief vulkan实例
*    与 vulkan交互的第一个环节，对应VkInstance
*    负责设置调试验证层 和 查询可用的物理设备。
*/

#ifndef ECAS_BACKEND_VULKAN_INSTANCE_HPP_
#define ECAS_BACKEND_VULKAN_INSTANCE_HPP_

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace ecas {
namespace vulkan {

class Instance {
public:
    Instance(bool enable_validation = false);
    ~Instance();

    inline std::vector<const char*> &layers() { return layers_; }
    // Enumerates all available physical devices on system.
    std::vector<VkPhysicalDevice> EnumeratePhysicalDevices(bool show_infos=false);

private:
    // Filter out unsupported extensions.
    static std::vector<const char*> 
    FilterOutEnabledExtensions(const std::vector<const char*>&& extensions);
    // Filter out unsupported layers.
    static std::vector<const char*> 
    FilterOutEnabledLayers(const std::vector<const char*>&& layers);

private:
    VkInstance instance_;
    // Set to true during the debugging phase.
    bool enable_validation_; 
    std::vector<const char*> layers_;
    std::vector<const char*> extensions_;
    VkDebugReportCallbackEXT debug_report_callback_;
};

}  // namespace vulkan
}  // namespace ecas

#endif  // ECAS_BACKEND_VULKAN_INSTANCE_HPP_
