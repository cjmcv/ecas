
#ifndef ECAS_BACKEND_VULKAN_INSTANCE_HPP_
#define ECAS_BACKEND_VULKAN_INSTANCE_HPP_

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace ecas {
namespace vulkan {

// This class is the beginning of all interaction with the Vulkan system. 
class Instance {
public:
    Instance(const char *app_name, bool enable_validation = false);
    ~Instance();

    inline std::vector<const char*> &layers() { return layers_; }
    // inline std::vector<const char*> &extensions() { return extensions_; }

    // Enumerates all available physical devices on system.
    std::vector<VkPhysicalDevice> EnumeratePhysicalDevices(bool show_infos=false);

private:
    static std::vector<const char*> 
    FilterOutEnabledExtensions(const std::vector<const char*>&& extensions);
    
    static std::vector<const char*> 
    FilterOutEnabledLayers(const std::vector<const char*>&& layers);

private:
    VkInstance instance_;
    bool enable_validation_;
    std::vector<const char*> layers_;
    std::vector<const char*> extensions_;
    VkDebugReportCallbackEXT debug_report_callback_;
};

}  // namespace vulkan
}  // namespace ecas

#endif  // ECAS_BACKEND_VULKAN_INSTANCE_HPP_
