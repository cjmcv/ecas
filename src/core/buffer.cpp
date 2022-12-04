/*!
* \brief Buffer. 
*/

#include "buffer.hpp"

namespace dlas {

Buffer::Buffer(unsigned int size) {
    data_ = malloc(size);
}

Buffer::~Buffer() {
    free(data_);
}

}  // end of namespace dlas.