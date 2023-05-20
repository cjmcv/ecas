
#include "vulkan/vulkan.h"

#include "ecas/ecas.hpp"
#include <cmath>

#include "common.hpp"
#include "util/bmp_reader.hpp"
#include "instance.hpp"
#include "device.hpp"
#include "buffer.hpp"
#include "shader_module.hpp"
#include "pipeline.hpp"
#include "descriptor_pool.hpp"
#include "command_buffer.hpp"

const int WIDTH = 3200; // Size of rendered mandelbrot set.
const int HEIGHT = 2400; // Size of renderered mandelbrot set.
const int WORKGROUP_SIZE = 32; // Workgroup size in compute shader.

namespace ecas {
namespace vulkan {

class ComputeApplication {
private:
    // The pixels of the rendered mandelbrot set are in this format:
    struct Pixel {
        float r, g, b, a;
    };
    
    VkDescriptorSet descriptorSet;

        
    uint32_t bufferSize; // size of `buffer` in bytes.

public:
    void run() {
        // Buffer size of the storage buffer that will contain the rendered mandelbrot set.
        bufferSize = sizeof(Pixel) * WIDTH * HEIGHT;

        Instance *ins = new Instance(true);
        std::vector<VkPhysicalDevice> phys_devices = ins->EnumeratePhysicalDevices(true);
        
        Device *dev = new Device(phys_devices[0], VK_QUEUE_COMPUTE_BIT, ins->layers());

        Buffer *buf = new Buffer(dev->device(),
                                 dev->memory_properties(),
                                 VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                                 bufferSize);
 
        std::vector<VkDescriptorType> type{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER};
        ShaderModule *shader_module = ShaderModule::Create(dev->device(), type, "../src/kernel/vulkan/shaders/comp.spv");

        // createComputePipeline();
        std::vector<Pipeline::SpecConstant> spec_constant = {
            // {0, Pipeline::SpecConstant::Type::u32, output_h},
            // {1, Pipeline::SpecConstant::Type::u32, output_w},
        };

        Pipeline *pip = Pipeline::Create(dev->device(), *shader_module, "main", spec_constant);
        auto pool_sizes = shader_module->CalculateDescriptorPoolSize();
        DescriptorPool *des_pool = DescriptorPool::Create(dev->device(), shader_module->num_sets(), pool_sizes);

        std::vector<VkDescriptorSetLayout> set_layouts = shader_module->descriptor_set_layouts();
        auto layout_set_map = des_pool->AllocateDescriptorSets(set_layouts);
        descriptorSet = layout_set_map[shader_module->GetDescriptorSetLayout(0)];

        ///////////////////
        {
            // Specify the buffer to bind to the descriptor.
            VkDescriptorBufferInfo descriptorBufferInfo = {};
            descriptorBufferInfo.buffer = buf->buffer();
            descriptorBufferInfo.offset = 0;
            descriptorBufferInfo.range = buf->buffer_size();

            VkWriteDescriptorSet writeDescriptorSet = {};
            writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSet.dstSet = descriptorSet; // write to this descriptor set.
            writeDescriptorSet.dstBinding = 0; // write to the first, and only binding.
            writeDescriptorSet.descriptorCount = 1; // update a single descriptor.
            writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; // storage buffer.
            writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;

            // perform the update of the descriptor set.
            vkUpdateDescriptorSets(dev->device(), 1, &writeDescriptorSet, 0, NULL);            
        }

        CommandBuffer *combuf = CommandBuffer::Create(dev->device(), dev->command_pool());
        combuf->Begin();
        combuf->BindPipelineAndDescriptorSets(pip, {descriptorSet});
        combuf->Dispatch((uint32_t)ceil(WIDTH / float(WORKGROUP_SIZE)), (uint32_t)ceil(HEIGHT / float(WORKGROUP_SIZE)), 1);
        combuf->End();

        dev->QueueSubmitAndWait(combuf->command_buffer());
        
        void *mapped_data = buf->MapMemory(0, bufferSize);
        saveRenderedImage(mapped_data);
        buf->UnmapMemory();

        delete des_pool;
        delete pip;
        delete buf;
        delete shader_module;
        delete dev;
        delete ins;

        printf("vulkan end.\n");
    }

    void saveRenderedImage(void* mappedMemory) {
        Pixel* pmappedMemory = (Pixel *)mappedMemory;

        // Get the color data from the buffer, and cast it to bytes.
        // We save the data to a vector.
        int channels = 4;
        util::BmpReader img(WIDTH, HEIGHT, channels);
        uint8_t *img_data = img.data();
        for (int i = 0; i < WIDTH*HEIGHT; i++) {
            img_data[i * channels + 0] = (unsigned char)(255.0f * (pmappedMemory[i].b));
            img_data[i * channels + 1] = (unsigned char)(255.0f * (pmappedMemory[i].g));
            img_data[i * channels + 2] = (unsigned char)(255.0f * (pmappedMemory[i].r));
            img_data[i * channels + 3] = (unsigned char)(255.0f * (pmappedMemory[i].a));
        }
        // util::BmpReader img("man4.bmp");
        img.Write("man5.bmp");
    }
};

}  // namespace vulkan


int VulkanMain() {
    vulkan::ComputeApplication app;
    app.run();
}

}  // end of namespace ecas.