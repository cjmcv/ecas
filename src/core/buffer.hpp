/*!
* \brief Buffer. 
*/

#ifndef ECAS_CORE_BUFFER_HPP_
#define ECAS_CORE_BUFFER_HPP_

#include <string>
#include <vector>

namespace ecas {

enum MemoryType {
    ONLY_ON_HOST = 0,
    ONLY_ON_DEVICE = 1,
    ON_HOST_AND_DEVICE
};

// TODO: 基类Buffer，派生X86，vulkan等子类。由Buffer类统一对Tensor提供服务。
// 内存管理，只管内存不管怎么用
class Buffer {

public:
    // Buffer(uint32_t size) {}
    // Buffer(uint32_t size, void *data) {}
    virtual ~Buffer() {};

    virtual void *data() = 0;
};

}  // end of namespace ecas.

#endif // ECAS_CORE_BUFFER_HPP_