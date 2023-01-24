/*!
* \brief TensorPool. 
*/

#include "tensor_pool.hpp"

namespace ecas {

Tensor *TensorPool::CreateTensor(std::vector<unsigned int> &shape) {
    Tensor *t = new Tensor(shape, tensors_.size());
    tensors_.push_back(t);
    return t;
}

}  // end of namespace ecas.