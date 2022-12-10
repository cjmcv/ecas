/*!
* \brief Tensor. 
*/

#include "tensor.hpp"

namespace dlas {

Tensor::Tensor(std::vector<unsigned int> &shape) {
    size_ = 1;
    for (int i=0; i < shape.size(); i++) {
        size_ *= shape[i];
    }
    if (size_ == 0)
        std::abort();
    
    shape_ = shape;
    buffer_ = new Buffer(size_);
}

Tensor::~Tensor() {
    delete buffer_;
}

}  // end of namespace dlas.