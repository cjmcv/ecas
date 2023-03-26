/*!
* \brief Buffer. 
*/

#ifndef ECAS_CORE_BUFFER_HPP_
#define ECAS_CORE_BUFFER_HPP_

#include <string>
#include <vector>

namespace ecas {

// TODO: 基类Buffer，派生X86，vulkan等子类。由Buffer类统一对Tensor提供服务。
// 内存管理，只管内存不管怎么用
class Buffer {

public:
    Buffer(uint32_t size, void *data = nullptr);
    ~Buffer();

    inline void *data() { return data_; }
    void SetDataPtr(void *data);

private:
    void Release();

private:
    uint32_t size_;
    void *data_;
    bool is_owned_;
};

}  // end of namespace ecas.

#endif // ECAS_CORE_BUFFER_HPP_