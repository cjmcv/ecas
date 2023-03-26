/*!
* \brief Buffer. 
*/

#include "buffer.hpp"

namespace ecas {

Buffer::Buffer(uint32_t size, void *data) {
    if (data != nullptr) {
        data_ = data;
        is_owned_ = false;      
    }
    else {
        data_ = malloc(size);
        is_owned_ = true;
    }
    size_ = size;
}

Buffer::~Buffer() {
    Release();
}

void Buffer::Release() {
    if (is_owned_ == true && data_ != nullptr) {
        free(data_);
        data_ = nullptr;
    }
}

void Buffer::SetDataPtr(void *data) {
    // Only one of the held memory and external memory can exist.
    // Before setting the external memory, release the memory held.
    Release();
    data_ = data;
    is_owned_ = false;
}

}  // end of namespace ecas.