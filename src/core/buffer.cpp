/*!
* \brief Buffer. 
*/

#include "buffer.hpp"

namespace ecas {

Buffer::Buffer(void *data, int size) {
    data_ = data;
    size_ = size;
    is_owned_ = false;
}

Buffer::Buffer(unsigned int size) {
    data_ = malloc(size);
    is_owned_ = true;
}

Buffer::~Buffer() {
    if (is_owned_ == true && data_ != nullptr)
        free(data_);
}

}  // end of namespace ecas.