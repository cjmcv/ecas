/*!
* \brief .
*/

#include "kernel_dispatcher.hpp"
#include "util/logger.hpp"

namespace ecas {
namespace vulkan {

void SetParamsMandelbrot(KernelParams *params) {
    params->buffer_type = {
        DESCRIPTOR_TYPE_STORAGE_BUFFER
    };
    params->spec_constant = {};
    params->push_constant_num = 0;
    params->workgroup_size[0] = 32; // TODO: 在comp中写死了，这里的设定只是计算dispatch的group数量，考虑用特化常量
    params->workgroup_size[1] = 32;
    params->workgroup_size[2] = 1;
}

void SetParamsMatMulTiledFp32(KernelParams *params) {
    params->buffer_type = {
        DESCRIPTOR_TYPE_STORAGE_BUFFER,
        DESCRIPTOR_TYPE_STORAGE_BUFFER,
        DESCRIPTOR_TYPE_STORAGE_BUFFER // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
    };

    params->spec_constant = {
        {0, 640},
        {1, 640},
        {2, 640},
    };
    params->push_constant_num = 0;
    params->workgroup_size[0] = 16;
    params->workgroup_size[1] = 1;
    params->workgroup_size[2] = 1;
}

void SetParamsEngineTest(KernelParams *params) {
    params->buffer_type = {
        DESCRIPTOR_TYPE_STORAGE_BUFFER,
        DESCRIPTOR_TYPE_STORAGE_BUFFER,
        DESCRIPTOR_TYPE_STORAGE_BUFFER // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
    };

    params->spec_constant = {
        {0, 160},
        {1, 320}};
    SpecializationConstant c;
    c.id = 2;
    c.value.f32 = 640.123f;
    params->spec_constant.push_back(c);

    params->push_constant_num = 2;
    params->workgroup_size[0] = 16;
    params->workgroup_size[1] = 1;
    params->workgroup_size[2] = 1;
}

/////////
VulkanKernelDispatcher::VulkanKernelDispatcher() {
    params_["mandelbrot"] = SetParamsMandelbrot;
    params_["matmul_tiled_fp32"] = SetParamsMatMulTiledFp32;
    params_["engine_test"] = SetParamsEngineTest;
}

KernelParams *VulkanKernelDispatcher::CreateKernelParams(std::string kernel_name) {
    KernelParams *params = new KernelParams;

    std::map<std::string, SetpParamsFuncs>::iterator iter;
    iter = params_.find(kernel_name);
    if(iter != params_.end()) {
        ECAS_LOGI("CreateKernelParams: %s.\n", kernel_name.c_str());
        iter->second(params);
    }
    else {
        printf("Can not find params.\n");
    }
    return params;
}

}  // end of namespace vulkan.
}  // end of namespace ecas.