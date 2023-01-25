/*!
* \brief TensorPool. 
*/

#include "tensor_pool.hpp"

namespace ecas {

#define ECAS_BLOCKING_QUEUE_SIZE 10

TensorPool::~TensorPool() {
    for (int i = 0; i < bq_pairs_.size(); i++) {
        BlockingQueuePair *bqp = bq_pairs_[i];
        while (!bqp->free.empty()) {
            Tensor *t;
            bqp->free.wait_and_pop(&t);
            delete t;
        }
        while (!bqp->full.empty()) {
            Tensor *t;
            bqp->full.wait_and_pop(&t);
            delete t;
        }
        delete bqp;
    }
    bq_pairs_.clear();
    std::vector<BlockingQueuePair *>().swap(bq_pairs_);
}

BlockingQueuePair *TensorPool::CreateBlockingQueue(std::vector<int> &shape) {
    BlockingQueuePair *bqp = new BlockingQueuePair;
    for (int i=0; i<ECAS_BLOCKING_QUEUE_SIZE; i++) {
        Tensor *t = new Tensor(shape);
        bqp->free.push(t);
    }
    bq_pairs_.push_back(bqp);
    return bqp;
}

Tensor *TensorPool::CreateTensor(std::vector<int> &shape) {
    Tensor *t = new Tensor(shape);
    tensors_.push_back(t);
    return t;
}

}  // end of namespace ecas.