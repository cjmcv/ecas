/*!
* \brief .
*/

#ifndef ECAS_KERNEL_VULKAN_KERNELS_LIST_HPP_
#define ECAS_KERNEL_VULKAN_KERNELS_LIST_HPP_

#include <string>
#include <vector>
#include <map>

namespace ecas {
namespace vulkan {

// 该文件无vulkan依赖，kernel中不带入vulkan相关内容，与vulkan engine解耦。
// kernel的配置只描述了kernel的相关信息，engine里include了kernels_list后将里面的配置转换为vulkan相关类型进行使用。
// vulkan -> op 依赖 engine 依赖 kernel+kernel dispatcher。 host -> op 依赖 kernel。
typedef enum DescriptorType {
    DESCRIPTOR_TYPE_STORAGE_IMAGE = 1, // 3,
    DESCRIPTOR_TYPE_UNIFORM_BUFFER = 2, // 6,
    DESCRIPTOR_TYPE_STORAGE_BUFFER = 3 // 7
} DescriptorType;

struct SpecializationConstant {
    uint32_t id;
    union {
        int32_t s32;
        uint32_t u32;
        float f32;
    } value;
};

// kernel固定参数，在kernel注册时指定，不需要依赖vulkan头文件和库
struct KernelParams {
    std::vector<DescriptorType> buffer_type;
    std::vector<SpecializationConstant> spec_constant;
    uint32_t push_constant_num;
    uint32_t workgroup_size[3];
};

// 1. 根据型号，确定线程布局。2. 选择spirv文件。
// 原则：尽可能不添加太多kernel外的内容，使这部分代码可以快速解耦使用到其他地方，也方便后面快速加入其他地方的代码。

typedef void (*SetpParamsFuncs)(KernelParams *params);

class VulkanKernelDispatcher {
public:
    //
    KernelParams *CreateKernelParams(std::string kernel_name);

	// Singleton mode. Only one KernelFactory exist.
	static VulkanKernelDispatcher *GetInstance() {
		static VulkanKernelDispatcher *dispatcher = new VulkanKernelDispatcher;
		return dispatcher;
	}

private:
	VulkanKernelDispatcher();

    std::map<std::string, SetpParamsFuncs> params_;
};


}  // end of namespace vulkan.
}  // end of namespace ecas.

#endif // ECAS_KERNEL_VULKAN_KERNELS_LIST_HPP_