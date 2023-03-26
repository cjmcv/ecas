/*!
* \brief TensorPool. 
*/

#include "tensor_pool.hpp"
#include "util/logger.hpp"

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

BlockingQueuePair *TensorPool::CreateBlockingQueue(std::vector<int> &shape, DataType type) {
    BlockingQueuePair *bqp = new BlockingQueuePair;
    for (int i=0; i<ECAS_BLOCKING_QUEUE_SIZE; i++) {
        Tensor *t = new Tensor(shape, type);
        bqp->free.push(t);
    }
    bq_pairs_.push_back(bqp);
    return bqp;
}

Tensor *TensorPool::CreateTensor(std::vector<int> &shape, DataType type, void *data) {
    Tensor *t = new Tensor(shape, type, data);
    tensors_.push_back(t);
    return t;
}

void TensorPool::PrintInfo() {
    ECAS_LOGS("TensorPool info:\n");
    for (int i = 0; i < bq_pairs_.size(); i++) {
        BlockingQueuePair *bqp = bq_pairs_[i];
        ECAS_LOGS("[%s, %s]: (full: %d, free: %d).\n", 
                  bqp->front_name.c_str(), bqp->rear_name.c_str(),
                  bqp->full.size(), bqp->free.size());
    }
}

void TensorPool::ExitAllBlockingQueue() {
    for (int i=0; i<bq_pairs_.size(); i++) {
        BlockingQueuePair *bqp = bq_pairs_[i];
        bqp->full.exit();
        bqp->free.exit();
    }
}

}  // end of namespace ecas.