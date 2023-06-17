/*!
* \brief Allocator. 
*/

#include "allocator.hpp"
#include "backend/buffer/host_buffer.hpp"
#include "backend/buffer/vulkan_buffer.hpp"
#include "util/logger.hpp"

namespace ecas {

#define ECAS_BLOCKING_QUEUE_SIZE 10

Allocator::~Allocator() {
    // BlockingQueue
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
    
    // Buffers
    for (int i=0; i<buffers_.size(); i++) {
        delete buffers_[i];
    }
    // Tensors
    for (int i=0; i<tensors_.size(); i++) {
        delete tensors_[i];
    }
}

Buffer* Allocator::CreateBuffer(MemoryType type, uint32_t size) {
    switch (type) {
        case ONLY_ON_HOST: 
            return new HostBuffer(size);
        // case ONLY_ON_DEVICE:
        //     return new Buffer();
        // case ON_HOST_AND_DEVICE:
        //     return new Buffer();
        default: 
            ECAS_LOGE("Buffer* Create -> type %d is not supported.\n", type);
    }
}

BlockingQueuePair *Allocator::CreateBlockingQueue(std::vector<int> &shape, DataType type) {
    BlockingQueuePair *bqp = new BlockingQueuePair;
    for (int i=0; i<ECAS_BLOCKING_QUEUE_SIZE; i++) {
        Tensor *t = new Tensor(shape, type);
        Buffer *buffer = CreateBuffer(ONLY_ON_HOST, t->size());
        t->BindBuffer(buffer);
        bqp->free.push(t);

        buffers_.push_back(buffer);
    }
    bq_pairs_.push_back(bqp);
    return bqp;
}

Tensor *Allocator::CreateTensor(std::vector<int> &shape, DataType type, void *data) {
    Tensor *t = new Tensor(shape, type);
    if (data != nullptr)
        t->BindHostDataPtr(data);
    else {
        Buffer *buffer = CreateBuffer(ONLY_ON_HOST, t->size());
        t->BindBuffer(buffer);
        buffers_.push_back(buffer);
    }
    tensors_.push_back(t);
    return t;
}

void Allocator::PrintInfo() {
    ECAS_LOGS("Allocator info:\n");
    for (int i = 0; i < bq_pairs_.size(); i++) {
        BlockingQueuePair *bqp = bq_pairs_[i];
        ECAS_LOGS("[%s, %s]: (full: %d, free: %d).\n", 
                  bqp->front_name.c_str(), bqp->rear_name.c_str(),
                  bqp->full.size(), bqp->free.size());
    }
}

void Allocator::ExitAllBlockingQueue() {
    for (int i=0; i<bq_pairs_.size(); i++) {
        BlockingQueuePair *bqp = bq_pairs_[i];
        bqp->full.exit();
        bqp->free.exit();
    }
}

}  // end of namespace ecas.