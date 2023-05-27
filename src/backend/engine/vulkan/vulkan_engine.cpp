
#include "vulkan/vulkan.h"

#include "ecas/ecas.hpp"
#include <cmath>

#include "util/bmp_reader.hpp"
#include "vulkan_engine.hpp"

namespace ecas {
namespace vulkan {

const int WIDTH = 3200; // Size of rendered mandelbrot set.
const int HEIGHT = 2400; // Size of renderered mandelbrot set.

// The pixels of the rendered mandelbrot set are in this format:
struct Pixel {
    float r, g, b, a;
};

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
    instance_ = new Instance(enable_validation);
    std::vector<VkPhysicalDevice> phys_devices = instance_->EnumeratePhysicalDevices(true);
    device_ = Device::Create(phys_devices[physical_device_id], VK_QUEUE_COMPUTE_BIT, instance_->layers());

    SetKernelMap();
    std::unordered_map<std::string, KernelResPack*>::iterator it = res_map_.begin();
    while (it != res_map_.end()) {
        KernelResPack *res = it->second;

        std::vector<VkDescriptorType> type{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER};
        std::string kernel_path = (std::string)"../src/kernel/vulkan/shaders/" + it->first + ".spv";
        ShaderModule *shader_module = ShaderModule::Create(device_->device(), type, kernel_path.c_str());
        //
        std::vector<VkDescriptorSetLayout> set_layouts = shader_module->descriptor_set_layouts();
        Pipeline *pipeline = Pipeline::Create(device_->device(), shader_module->shader_module(), 
                                              set_layouts, "main", res->spec_constant, res->push_constant_num); 
        //
        auto pool_sizes = shader_module->CalculateDescriptorPoolSize();
        DescriptorPool *descriptor_pool = DescriptorPool::Create(device_->device(), shader_module->num_sets(), pool_sizes);
        descriptor_pool->AllocateDescriptorSets(set_layouts);
        VkDescriptorSet descriptor_set = descriptor_pool->GetDescriptorSet(set_layouts[0]);
        //
        uint32_t buffer_size = sizeof(Pixel) * WIDTH * HEIGHT;
        Buffer *buffer = Buffer::Create(device_->device(),
                                     device_->memory_properties(),
                                     VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                                     buffer_size);

        descriptor_pool->WriteBuffer(descriptor_set, 0, buffer);
        //
        CommandBuffer *command_buffer = CommandBuffer::Create(device_->device(), device_->command_pool());
        command_buffer->Begin();
        command_buffer->BindPipelineAndDescriptorSets(pipeline, {descriptor_set});
        int WORKGROUP_SIZE = 32; // Workgroup size in compute shader.
        command_buffer->Dispatch((uint32_t)ceil(WIDTH / float(WORKGROUP_SIZE)), (uint32_t)ceil(HEIGHT / float(WORKGROUP_SIZE)), 1);
        command_buffer->End();

        //
        res->buffer_size = buffer_size;
        res->command_buffer = command_buffer;
        res->buffer = buffer;
        res->descriptor_pool = descriptor_pool;
        res->pipeline = pipeline;
        res->shader_module = shader_module;
        printf("Finish VulkanEngine::Init.\n");

        it++;
    }
}

void VulkanEngine::Deinit() {
    // Cleanup res_map_
    std::unordered_map<std::string, KernelResPack*>::iterator it = res_map_.begin();
    while (it != res_map_.end()) {
        KernelResPack *res = it->second;
        delete res->command_buffer;
        delete res->buffer;
        delete res->descriptor_pool;
        delete res->pipeline;
        delete res->shader_module;
    }

    delete device_;
    delete instance_;
}

void VulkanEngine::SetKernelMap() {
    {
        KernelResPack *res = new KernelResPack;
        res->spec_constant = {
            // {0, Pipeline::SpecConstant::Type::u32, output_h},
            // {1, Pipeline::SpecConstant::Type::u32, output_w},
        };
        res->push_constant_num = 0;
        res_map_["mandelbrot"] = res;
    }
}

void VulkanEngine::Run(std::string kernel_name) {
    std::unordered_map<std::string, KernelResPack*>::iterator it = res_map_.find(kernel_name);
    if (it == res_map_.end()) {
        ECAS_LOGE("Can not find Op: %s.\n", kernel_name);
        return;
    }
    KernelResPack *res = it->second;

    printf("1.\n");
    device_->QueueSubmitAndWait(res->command_buffer->command_buffer());

    void *mapped_data = res->buffer->MapMemory(0, res->buffer_size);
    static int idx = 0;
    idx++;
    printf("round idx: %d.\n", idx);
    saveRenderedImage(mapped_data, idx);
    res->buffer->UnmapMemory();
}

}  // namespace vulkan


int VulkanMain() {
    vulkan::VulkanEngine engine;
    engine.Init(true);
    for (int i=0; i<10; i++)
        engine.Run("mandelbrot");
}

}  // end of namespace ecas.