/*!
* \brief TensorPool. 
*/

#ifndef ECAS_CORE_TENSOR_POOL_HPP_
#define ECAS_CORE_TENSOR_POOL_HPP_

#include "tensor.hpp"
#include "util/blocking_queue.hpp"


namespace ecas {

struct BlockingQueuePair {
    util::BlockingQueue<Tensor *> free_;
    util::BlockingQueue<Tensor *> full_;
};

class TensorPool {
public:
    void CreateBlockingQueuePair() {

    }
    Tensor *CreateTensor(std::vector<unsigned int> &shape);

private:
    std::vector<BlockingQueuePair *> queue_pairs_;
    std::vector<Tensor *> tensors_;
};

}  // end of namespace ecas.

#endif // ECAS_CORE_TENSOR_POOL_HPP_