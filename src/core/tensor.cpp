/*!
* \brief Tensor. 
*/

#include "tensor.hpp"

namespace ecas {

Tensor::Tensor(std::vector<unsigned int> &shape, int id) {
    id_ = id;
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

}  // end of namespace ecas.