/*!
* \brief TensorPool. 
*/

#ifndef ECAS_CORE_TENSOR_POOL_HPP_
#define ECAS_CORE_TENSOR_POOL_HPP_

#include "tensor.hpp"
#include "util/blocking_queue.hpp"


namespace ecas {

// TODO: 兼并同步模式，不使用blocking_queue. 提供统一对外的结构体
struct BlockingQueuePair {
    util::BlockingQueue<Tensor *> free;
    util::BlockingQueue<Tensor *> full;
};

class TensorPool {
public:
    ~TensorPool();
    BlockingQueuePair *CreateBlockingQueue(std::vector<int> &shape);
    Tensor *CreateTensor(std::vector<int> &shape);

private:
    std::vector<BlockingQueuePair *> bq_pairs_;
    std::vector<Tensor *> tensors_;
};

}  // end of namespace ecas.

#endif // ECAS_CORE_TENSOR_POOL_HPP_