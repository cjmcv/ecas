
#include "vulkan/vulkan.h"

#include "ecas/ecas.hpp"
#include <cmath>

#include "util/bmp_reader.hpp"
#include "vulkan_engine.hpp"

const int WIDTH = 3200; // Size of rendered mandelbrot set.
const int HEIGHT = 2400; // Size of renderered mandelbrot set.

// The pixels of the rendered mandelbrot set are in this format:
struct Pixel {
    float r, g, b, a;
};

namespace ecas {
namespace vulkan {

void saveRenderedImage(void *mappedMemory, int idx) {
    Pixel *pmappedMemory = (Pixel *)mappedMemory;

    // Get the color data from the buffer, and cast it to bytes.
    // We save the data to a vector.
    int channels = 4;
    util::BmpReader img(WIDTH, HEIGHT, channels);
    uint8_t *img_data = img.data();
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        img_data[i * channels + 0] = (unsigned char)(255.0f * (pmappedMemory[i].b));
        img_data[i * channels + 1] = (unsigned char)(255.0f * (pmappedMemory[i].g));
        img_data[i * channels + 2] = (unsigned char)(255.0f * (pmappedMemory[i].r));
        img_data[i * channels + 3] = (unsigned char)(255.0f * (pmappedMemory[i].a));
    }
    std::string img_name = std::to_string(idx) + ".bmp";
    img.Write(img_name.c_str());
}

void VulkanEngine::Init(int physical_device_id, bool enable_validation) {
    vk_dispatcher_ = VulkanKernelDispatcher::GetInstance();
    instance_ = new Instance(enable_validation);
    std::vector<VkPhysicalDevice> phys_devices = instance_->EnumeratePhysicalDevices(true);
    device_ = Device::Create(phys_devices[physical_device_id], VK_QUEUE_COMPUTE_BIT, instance_->layers());

    SetKernelMap();
    std::unordered_map<std::string, ExecUnit*>::iterator it = exec_map_.begin();
    while (it != exec_map_.end()) {
        ExecUnit *res = it->second;
        KernelParams *params = res->params;

        res->device_ = device_;
        std::string kernel_path = (std::string)"../src/kernel/vulkan/shaders/" + it->first + ".spv";
        std::vector<VkDescriptorType> buffer_types;
        buffer_types.resize(params->buffer_type.size());
        for (uint32_t i=0; i<params->buffer_type.size(); i++) {
            if (params->buffer_type[i] == DESCRIPTOR_TYPE_STORAGE_BUFFER)
                buffer_types[i] = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        }
        res->shader_module_ = ShaderModule::Create(device_->device(), buffer_types, kernel_path.c_str());
        //
        std::vector<SpecConstant> spec_constants;
        spec_constants.resize(params->spec_constant.size());
        for (uint32_t i=0; i<params->spec_constant.size(); i++) {
            spec_constants[i].id = params->spec_constant[i].id;
            spec_constants[i].value.u32 = params->spec_constant[i].value.u32;
        }
        std::vector<VkDescriptorSetLayout> set_layouts = res->shader_module_->descriptor_set_layouts();
        res->pipeline_ = Pipeline::Create(device_->device(), res->shader_module_->shader_module(), 
                                          set_layouts, "main", spec_constants, params->push_constant_num); 
        //
        auto pool_sizes = res->shader_module_->CalculateDescriptorPoolSize();
        res->descriptor_pool_ = DescriptorPool::Create(device_->device(), res->shader_module_->num_sets(), pool_sizes);
        res->descriptor_pool_->AllocateDescriptorSets(set_layouts);
        res->descriptor_set_ = res->descriptor_pool_->GetDescriptorSet(set_layouts[0]);

        res->command_buffer_ = CommandBuffer::Create(device_->device(), device_->command_pool());

        it++;
    } 
    printf("Finish VulkanEngine::Init.\n");
}

void VulkanEngine::Deinit() {
    // Cleanup exec_map_
    std::unordered_map<std::string, ExecUnit*>::iterator it = exec_map_.begin();
    while (it != exec_map_.end()) {
        ExecUnit *res = it->second;
        delete res->command_buffer_;
        delete res->descriptor_pool_;
        delete res->pipeline_;
        delete res->shader_module_;
        delete res->params;
        delete res;
        it++;
    }
    delete device_;
    delete instance_;
}

void VulkanEngine::SetKernelMap() {
    {
        ExecUnit *res = new ExecUnit;
        res->params = vk_dispatcher_->CreateKernelParams("mandelbrot");
        exec_map_["mandelbrot"] = res;
    }
    {
        ExecUnit *res = new ExecUnit;
        res->params = vk_dispatcher_->CreateKernelParams("matmul_tiled_fp32");
        exec_map_["matmul_tiled_fp32"] = res;
    }
    {
        ExecUnit *res = new ExecUnit;
        res->params = vk_dispatcher_->CreateKernelParams("engine_test");
        exec_map_["engine_test"] = res;
    }
}

Buffer *VulkanEngine::CreateBuffer(uint32_t size) {
    return Buffer::Create(device_->device(),
                          device_->memory_properties(),
                          VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                          size);
}

void VulkanEngine::Run(std::string kernel_name, std::vector<Buffer*> &input_buffers, std::vector<Buffer*> &output_buffers) {
    std::unordered_map<std::string, ExecUnit*>::iterator it = exec_map_.find(kernel_name);
    if (it == exec_map_.end()) {
        // ECAS_LOGE("Can not find Op: %s.\n", kernel_name.c_str());
        printf("Can not find Op: %s.\n", kernel_name.c_str());
        return;
    }
    ExecUnit *res = it->second;
    res->Run(input_buffers, output_buffers);
}

/////

void VulkanEngine::ExecUnit::Run(std::vector<Buffer*> &input_buffers, std::vector<Buffer*> &output_buffers) {
    static int idx = 0;
    idx++;
    printf("round idx: %d.\n", idx);

    // buffer绑定顺序需要跟comp里一致
    for (int32_t i=0; i<input_buffers.size(); i++)
        descriptor_pool_->WriteBuffer(descriptor_set_, i, input_buffers[i]);
    for (int32_t i=0; i<output_buffers.size(); i++)
        descriptor_pool_->WriteBuffer(descriptor_set_, input_buffers.size() + i, output_buffers[i]);

    command_buffer_->Begin();
    command_buffer_->BindPipelineAndDescriptorSets(pipeline_, {descriptor_set_});
    command_buffer_->Dispatch((uint32_t)ceil(WIDTH / float(params->workgroup_size[0])), 
                              (uint32_t)ceil(HEIGHT / float(params->workgroup_size[1])), 
                              1);
    command_buffer_->End();

    device_->QueueSubmitAndWait(command_buffer_->command_buffer());
}

}  // namespace vulkan


int VulkanMain() {
    printf("VulkanMain Start.\n");

    vulkan::VulkanEngine engine;
    engine.Init(0, true);

    {
        uint32_t size = sizeof(Pixel) * WIDTH * HEIGHT;
        vulkan::Buffer *buf = engine.CreateBuffer(size);
        std::vector<vulkan::Buffer *> input_buffers = { buf };
        std::vector<vulkan::Buffer *> output_buffers = {};
        for (uint32_t i=0; i<2; i++) {
            engine.Run("mandelbrot", input_buffers, output_buffers);
            {
                vulkan::Buffer *buffer = input_buffers[0];
                void *mapped_data = buffer->MapMemory(0, buffer->buffer_size());
                vulkan::saveRenderedImage(mapped_data, i);
                buffer->UnmapMemory();
            }
        }
        delete buf;
    }

    {
        uint32_t len = 640 * 640;
        uint32_t size = sizeof(float) * len;
        vulkan::Buffer *input_buffer0 = engine.CreateBuffer(size);
        vulkan::Buffer *input_buffer1 = engine.CreateBuffer(size);
        vulkan::Buffer *output_buffer = engine.CreateBuffer(size);
        std::vector<vulkan::Buffer *> input_buffers = { input_buffer0, input_buffer1 };
        std::vector<vulkan::Buffer *> output_buffers = { output_buffer };

        float *mapped_data0 = (float *)input_buffer0->MapMemory(0, input_buffer0->buffer_size());
        float *mapped_data1 = (float *)input_buffer1->MapMemory(0, input_buffer1->buffer_size());
        for (uint32_t i=0; i<len; i++) {
            mapped_data0[i] = 1;
            mapped_data1[i] = 1;
        }
        input_buffer0->UnmapMemory();
        input_buffer1->UnmapMemory();

        for (uint32_t i=0; i<1; i++) {
            engine.Run("matmul_tiled_fp32", input_buffers, output_buffers);

            {
                vulkan::Buffer *buffer = output_buffers[0];
                float *mapped_data = (float *)buffer->MapMemory(0, buffer->buffer_size());
                for (uint32_t i=0; i<2; i++) { // 640
                    for (uint32_t j=0; j<2; j++) { // 640
                        printf("%f, ", mapped_data[i*640+j]);
                    }
                    printf("\n");
                }
                buffer->UnmapMemory();
            }   
        }
        
        for (int32_t i=0; i<input_buffers.size(); i++)
            delete input_buffers[i];
        for (int32_t i=0; i<output_buffers.size(); i++)
            delete output_buffers[i];
    }

    {
        uint32_t len = 640 * 640;
        uint32_t size = sizeof(float) * len;
        vulkan::Buffer *input_buffer0 = engine.CreateBuffer(size);
        vulkan::Buffer *input_buffer1 = engine.CreateBuffer(size);
        vulkan::Buffer *output_buffer = engine.CreateBuffer(size);
        std::vector<vulkan::Buffer *> input_buffers = { input_buffer0, input_buffer1 };
        std::vector<vulkan::Buffer *> output_buffers = { output_buffer };

        float *mapped_data0 = (float *)input_buffer0->MapMemory(0, input_buffer0->buffer_size());
        float *mapped_data1 = (float *)input_buffer1->MapMemory(0, input_buffer1->buffer_size());
        for (uint32_t i=0; i<len; i++) {
            mapped_data0[i] = 1;
            mapped_data1[i] = 1;
        }
        input_buffer0->UnmapMemory();
        input_buffer1->UnmapMemory();

        for (uint32_t i=0; i<1; i++) {
            engine.Run("engine_test", input_buffers, output_buffers);

            {
                vulkan::Buffer *buffer = output_buffers[0];
                float *mapped_data = (float *)buffer->MapMemory(0, buffer->buffer_size());
                printf("engine_test0: (%f, %f, %f, %f)\n", mapped_data[0], mapped_data[1], mapped_data[2], mapped_data[3]);
                printf("engine_test1: (%f, %f, %f, %f)\n", mapped_data[4], mapped_data[5], mapped_data[6], mapped_data[7]);
                buffer->UnmapMemory();
            }   
        }
        
        for (int32_t i=0; i<input_buffers.size(); i++)
            delete input_buffers[i];
        for (int32_t i=0; i<output_buffers.size(); i++)
            delete output_buffers[i];
    }

    engine.Deinit();

    printf("VulkanMain End.\n");

    return 0;
}

}  // end of namespace ecas.