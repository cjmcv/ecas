

#include "instance.hpp"

#include <memory>
#include <string.h>
#include "util/logger.hpp"
#include "common.hpp"

namespace ecas {
namespace vulkan {

std::vector<const char*> 
Instance::FilterOutEnabledExtensions(const std::vector<const char*>&& extensions) {
    auto ret = std::vector<const char*>{};

    uint32_t extension_count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
    std::vector<VkExtensionProperties> ins_exts(extension_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, ins_exts.data());

    for (auto e : extensions) {
        bool is_exist = false;
        for (auto ie : ins_exts) {
            if (!strcmp(ie.extensionName, e)) {
                ret.push_back(e);
                is_exist = true;
                break;
            }
        }
        if (!is_exist) {
            ECAS_LOGW("[WARNING] extension %s can not be found. \n", e);
        }
    }

    // Not all extension are supported.
    if (ret.size() != extensions.size()) {
        ECAS_LOGS("Supported extensions: \n");
        for (auto ie : ins_exts) {
            ECAS_LOGS("%s.\n", ie.extensionName);
        }
    }
    return ret; //  move construct
}

std::vector<const char*> 
Instance::FilterOutEnabledLayers(const std::vector<const char*>&& layers) {
    auto ret = std::vector<const char*>{};

    uint32_t layer_count = 0;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
    std::vector<VkLayerProperties> ins_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, ins_layers.data());

    for (auto l : layers) {
        bool is_exist = false;
        for (auto il : ins_layers) {
            if (!strcmp(il.layerName, l)) {
                ret.push_back(l);
                is_exist = true;
                break;
            }
        }
        if (!is_exist) {
            ECAS_LOGW("[WARNING] layer %s can not be found. \n", l);
        }
    }

    // Not all layers are supported.
    if (ret.size() != layers.size()) {
        ECAS_LOGS("Supported layers: \n");
        for (auto il : ins_layers) {
            ECAS_LOGS("%s.\n", il.layerName);
        }
    }
    return ret;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallbackFn(
    VkDebugReportFlagsEXT flags,

    VkDebugReportObjectTypeEXT objectType,
    uint64_t object,
    size_t location,
    int32_t messageCode,
    const char *pLayerPrefix,
    const char *pMessage,
    void *pUserData) {

    printf("Debug Report: %s: %s\n", pLayerPrefix, pMessage);

    return VK_FALSE;
}

Instance::Instance(const char *app_name, bool enable_validation) {
    enable_validation_ = enable_validation;

    layers_.clear();
    extensions_.clear();
    if (enable_validation) {
        // Note: VK_LAYER_LUNARG_standard_validation is deprecated.
        layers_ = FilterOutEnabledLayers({ "VK_LAYER_KHRONOS_validation" });
        extensions_ = FilterOutEnabledExtensions({ "VK_EXT_debug_report" });
    }

    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pNext = nullptr;
    app_info.pApplicationName = app_name;
    app_info.applicationVersion = 0;
    app_info.pEngineName = "EcasVulkan";
    app_info.engineVersion = 0;
    app_info.apiVersion = VK_API_VERSION_1_1;

    VkInstanceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.pApplicationInfo = &app_info;
    create_info.enabledLayerCount = layers_.size();
    create_info.ppEnabledLayerNames = layers_.data();
    create_info.enabledExtensionCount = extensions_.size();
    create_info.ppEnabledExtensionNames = extensions_.data();

    // printf("le: %d, %s, %d, %s.\n", layers.size(), layers[0], extensions.size(), extensions[0]);
    instance_ = VK_NULL_HANDLE;
    vkCreateInstance(&create_info, /*pAllocator=*/nullptr, &instance_);

    // Register a callback function for the extension VK_EXT_debug_report, so that warnings emitted from the validation
    // layer are actually printed.
    if (enable_validation) {
        VkDebugReportCallbackCreateInfoEXT callback_create_info = {};
        callback_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        callback_create_info.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
        callback_create_info.pfnCallback = &DebugReportCallbackFn;

        // We have to explicitly load this function.
        auto vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance_, "vkCreateDebugReportCallbackEXT");
        if (vkCreateDebugReportCallbackEXT == nullptr) {
            throw std::runtime_error("Could not load vkCreateDebugReportCallbackEXT");
        }
        // Create and register callback.
        VK_CHECK(vkCreateDebugReportCallbackEXT(instance_, &callback_create_info, NULL, &debug_report_callback_));
    }
}

Instance::~Instance() {
    if (enable_validation_) {
        // destroy callback.
        auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance_, "vkDestroyDebugReportCallbackEXT");
        if (func == nullptr) {
            throw std::runtime_error("Could not load vkDestroyDebugReportCallbackEXT");
        }
        func(instance_, debug_report_callback_, NULL);
    }

    vkDestroyInstance(instance_, /*pAllocator=*/nullptr);
}

std::vector<VkPhysicalDevice> Instance::EnumeratePhysicalDevices(bool show_infos) {
    uint32_t count = 0;
    vkEnumeratePhysicalDevices(instance_, &count, nullptr);

    std::vector<VkPhysicalDevice> devices(count);
    vkEnumeratePhysicalDevices(instance_, &count, devices.data());

    // Display properties of physical devices
    if (show_infos) {
        for (int i = 0; i < count; ++i) {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(devices[i], &properties);

            ///////////////////////////////////
            //  Information.
            ECAS_LOGS("\n//////////////// Physical Device: %d //////////////////\n", i);
            std::string type;
            if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_OTHER)
                type = "VK_PHYSICAL_DEVICE_TYPE_OTHER";
            else if(properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
                type = "VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU";
            else if (properties.deviceType== VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                type = "VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU";
            else if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU)
                type = "VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU";
            else if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU)
                type = "VK_PHYSICAL_DEVICE_TYPE_CPU";
            ECAS_LOGS("device type: %s\n", type.c_str());
            ECAS_LOGS("device name: %s \n", properties.deviceName);
            ECAS_LOGS("api version: %d \n", properties.apiVersion);
            ECAS_LOGS("driver version: %d \n", properties.driverVersion);
            ECAS_LOGS("vendor id: %d \n", properties.vendorID);
            ECAS_LOGS("device id: %d \n", properties.deviceID);

            ///////////////////////////////////
            // Capability
            ECAS_LOGS("max compute shared memory size: %d\n", properties.limits.maxComputeSharedMemorySize);
            ECAS_LOGS("max workgroup count: [%d, %d, %d]\n", properties.limits.maxComputeWorkGroupCount[0],
                                                             properties.limits.maxComputeWorkGroupCount[1], 
                                                             properties.limits.maxComputeWorkGroupCount[2]);
            ECAS_LOGS("max workgroup invocations: %d\n", properties.limits.maxComputeWorkGroupInvocations);
            ECAS_LOGS("max workgroup size: [%d, %d, %d]\n", properties.limits.maxComputeWorkGroupSize[0],
                                                            properties.limits.maxComputeWorkGroupSize[1],
                                                            properties.limits.maxComputeWorkGroupSize[2]);

            ECAS_LOGS("memory map alignment: %d\n", properties.limits.minMemoryMapAlignment);
            ECAS_LOGS("buffer offset alignment: %d\n", properties.limits.minStorageBufferOffsetAlignment);
        }
    }
    return devices;
}

}  // namespace vulkan
}  // namespace ecas
