#include <iostream>
#include <vector>

#include "vulkan/vulkan.h"
// #include <stdint.h>


void CreateInstance(bool is_enable_validation, VkInstance *instance) {

  std::vector<const char*> layers = std::vector<const char*>{};
  std::vector<const char*> extensions = std::vector<const char*>{};
//   if (is_enable_validation) {
//     // "=" in vector is deep copy. 
//     // Note: VK_LAYER_LUNARG_standard_validation is deprecated.
//     layers = EnabledLayers({ "VK_LAYER_KHRONOS_validation" });
//     // The same as VK_EXT_DEBUG_REPORT_EXTENSION_NAME
//     extensions = EnabledExtensions({ "VK_EXT_debug_report" });
//   }

  VkApplicationInfo app_info = {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "Vulkan Compute Example";
  app_info.applicationVersion = 0;
  app_info.pEngineName = "no_engine";
  app_info.engineVersion = 0;
  app_info.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.flags = VkInstanceCreateFlags();
  create_info.pApplicationInfo = &app_info;
  create_info.enabledLayerCount = layers.size();
  create_info.ppEnabledLayerNames = layers.data();
  create_info.enabledExtensionCount = extensions.size();
  create_info.ppEnabledExtensionNames = extensions.data();

  vkCreateInstance(&create_info, 0, instance);
}

void DestroyInstance(VkInstance &instance) {
  vkDestroyInstance(instance, NULL);
}

//

class DeviceInfo {
public:
  VkPhysicalDevice physical_device_;

  // info
  char device_name_[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE];
  uint32_t api_version_;
  uint32_t driver_version_;
  uint32_t vendor_id_;
  uint32_t device_id_;

  // eOther = VK_PHYSICAL_DEVICE_TYPE_OTHER,
  // eIntegratedGpu = VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU,
  // eDiscreteGpu = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
  // eVirtualGpu = VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU,
  // eCpu = VK_PHYSICAL_DEVICE_TYPE_CPU
  VkPhysicalDeviceType type_;

  // hardware capability
  uint32_t max_shared_memory_size_;
  uint32_t max_workgroup_count_[3];
  uint32_t max_workgroup_invocations_;
  uint32_t max_workgroup_size_[3];

  uint32_t memory_map_alignment_;
  uint32_t buffer_offset_alignment_;

  // runtime
  uint32_t compute_queue_familly_id_;
};

void QueryDeviceInfo(VkInstance &instance, std::vector<DeviceInfo> &devices_info) {

  uint32_t device_count;
  vkEnumeratePhysicalDevices(instance, &device_count, NULL);
  if (device_count == 0) {
    throw std::runtime_error("could not find a device with vulkan support");
  }

  std::vector<VkPhysicalDevice> physical_devices;
  physical_devices.resize(device_count);
  vkEnumeratePhysicalDevices(instance, &device_count, physical_devices.data());

  devices_info.resize(device_count);
  for (uint32_t i = 0; i < device_count; i++) {
    const VkPhysicalDevice& physical_device = physical_devices[i];
    DeviceInfo &info = devices_info[i];

    VkPhysicalDeviceProperties device_properties;
    // physical_device->getProperties(&device_properties);
    vkGetPhysicalDeviceProperties(physical_device, &device_properties);

    info.physical_device_ = physical_device;

    // info
    memcpy(info.device_name_, device_properties.deviceName, VK_MAX_PHYSICAL_DEVICE_NAME_SIZE * sizeof(char));
    info.api_version_ = device_properties.apiVersion;
    info.driver_version_ = device_properties.driverVersion;
    info.vendor_id_ = device_properties.vendorID;
    info.device_id_ = device_properties.deviceID;
    info.type_ = device_properties.deviceType;

    // capability
    info.max_shared_memory_size_ = device_properties.limits.maxComputeSharedMemorySize;

    info.max_workgroup_count_[0] = device_properties.limits.maxComputeWorkGroupCount[0];
    info.max_workgroup_count_[1] = device_properties.limits.maxComputeWorkGroupCount[1];
    info.max_workgroup_count_[2] = device_properties.limits.maxComputeWorkGroupCount[2];

    info.max_workgroup_invocations_ = device_properties.limits.maxComputeWorkGroupInvocations;

    info.max_workgroup_size_[0] = device_properties.limits.maxComputeWorkGroupSize[0];
    info.max_workgroup_size_[1] = device_properties.limits.maxComputeWorkGroupSize[1];
    info.max_workgroup_size_[2] = device_properties.limits.maxComputeWorkGroupSize[2];

    info.memory_map_alignment_ = device_properties.limits.minMemoryMapAlignment;
    info.buffer_offset_alignment_ = device_properties.limits.minStorageBufferOffsetAlignment;

    // info.compute_queue_familly_id_ = GetComputeQueueFamilyId(physical_device);
  }
}

void PrintDevicesInfo(std::vector<DeviceInfo> &devices_info) {
  std::cout << "Number of devices: " << devices_info.size() << std::endl;

  for (int i = 0; i < devices_info.size(); i++) {
    const DeviceInfo &info = devices_info[i];

    ///////////////////////////////////
    //  Information.
    std::cout << std::endl;
    std::cout << "////////////////device:" << i << "//////////////////" << std::endl;
    std::cout << "device name: " << info.device_name_ << std::endl;
    std::cout << "api version: " << info.api_version_ << std::endl;
    std::cout << "driver version: " << info.driver_version_ << std::endl;
    std::cout << "vendor id: " << info.vendor_id_ << std::endl;
    std::cout << "device id: " << info.device_id_ << std::endl;

    std::string type;
    if (info.type_ == VK_PHYSICAL_DEVICE_TYPE_OTHER)
      type = "Other";
    else if(info.type_ == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
      type = "IntegratedGpu";
    else if (info.type_ == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
      type = "DiscreteGpu";
    else if (info.type_ == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU)
      type = "VirtualGpu";
    else if (info.type_ == VK_PHYSICAL_DEVICE_TYPE_CPU)
      type = "Cpu";
    std::cout << "device type: " << type << std::endl;

    ///////////////////////////////////
    // Capability
    std::cout << "max compute shared memory size: " << info.max_shared_memory_size_ << std::endl;
    std::cout << "max workgroup count: [" << info.max_workgroup_count_[0]
                                  << ", " << info.max_workgroup_count_[1] 
                                  << ", " << info.max_workgroup_count_[2] << "]" << std::endl;
    std::cout << "max workgroup invocations: " << info.max_workgroup_invocations_ << std::endl;
    std::cout << "max workgroup size: [" << info.max_workgroup_size_[0]
                                 << ", " << info.max_workgroup_size_[1]
                                 << ", " << info.max_workgroup_size_[2] << "]" << std::endl;

    std::cout << "memory map alignment: " << info.memory_map_alignment_ << std::endl;
    std::cout << "buffer offset alignment: " << info.buffer_offset_alignment_ << std::endl;
    std::cout << "compute queue familly id: " << info.compute_queue_familly_id_ << std::endl;
  }

  std::cout << std::endl << "//////////////////////////////////////////" << std::endl;
}


void VulkanHelloWorld() {
	
	VkInstance instance;
	CreateInstance(true, &instance);

	std::vector<DeviceInfo> devices_info;
	QueryDeviceInfo(instance, devices_info);
	PrintDevicesInfo(devices_info);

	DestroyInstance(instance);
}