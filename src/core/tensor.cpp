/*!
* \brief Tensor. 
*/

#include "tensor.hpp"

namespace ecas {

Tensor::Tensor(std::vector<int> &shape) {
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

void Tensor::CloneTo(Tensor *out) {
    out->size_ = this->size_;
    out->shape_ = this->shape_;
    memcpy(out->buffer_->data(), this->buffer_->data(), this->size_);
}

}  // end of namespace ecas.