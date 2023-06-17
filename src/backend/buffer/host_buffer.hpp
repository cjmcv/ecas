/*!
* \brief HostBuffer
*        host端buffer实现类
*        1. 自己开的内存; 2. 外部设的内存。
*/

#ifndef ECAS_BACKEND_EXTERNAL_HOST_BUFFER_HPP_
#define ECAS_BACKEND_EXTERNAL_HOST_BUFFER_HPP_

#include "core/buffer.hpp"

namespace ecas {

class HostBuffer: public Buffer {
public:
    HostBuffer(uint32_t size);
    HostBuffer(uint32_t size, void *data);
    ~HostBuffer();

    inline void *data() { return data_; }

private:
    uint32_t size_;
    void *data_;
    bool is_owned_;
};

}  // end of namespace ecas

#endif  // ECAS_BACKEND_EXTERNAL_VULKAN_BUFFER_HPP_
