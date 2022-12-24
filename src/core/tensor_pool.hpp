/*!
* \brief TensorPool. 
*/

#ifndef ECAS_CORE_TENSOR_POOL_HPP_
#define ECAS_CORE_TENSOR_POOL_HPP_

#include "tensor.hpp"
#include "util/blocking_queue.hpp"


namespace ecas {

struct TensorPack {
    int id;
    Tensor *tensor;
};

struct BlockingQueuePair {
    util::BlockingQueue<TensorPack *> free_;
    util::BlockingQueue<TensorPack *> full_;
};

class TensorPool {
public:
    void GenerateBlockingQueuePair() {

    }

private:
    
};

}  // end of namespace ecas.

#endif // ECAS_CORE_TENSOR_POOL_HPP_