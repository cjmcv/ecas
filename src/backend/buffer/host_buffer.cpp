
#include "host_buffer.hpp"

#include <string.h>
#include "util/logger.hpp"

namespace ecas {

HostBuffer::HostBuffer(uint32_t size) {
    data_ = nullptr;
    data_ = malloc(size);
    memset(data_, 0, size);
    size_ = size;
    is_owned_ = true;
}

HostBuffer::HostBuffer(uint32_t size, void *data) {
    data_ = data;
    size_ = size;
    is_owned_ = false;
}

HostBuffer::~HostBuffer() {
    if (is_owned_ == true && data_ != nullptr) {
        free(data_);
        data_ = nullptr;
    }
}

}  // namespace ecas
