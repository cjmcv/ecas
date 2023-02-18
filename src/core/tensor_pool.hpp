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
    std::string front_name;
    std::string rear_name;
    util::BlockingQueue<Tensor *> free;
    util::BlockingQueue<Tensor *> full;

    void Enqueue(ITensor *input) {
        Tensor *inside_free;
        free.wait_and_pop(&inside_free);
        inside_free->CopyFrom(input);
        full.push(inside_free);
    }

    void Dequeue(ITensor *output) {
        Tensor *inside_full;
        full.wait_and_pop(&inside_full);
        inside_full->CopyTo(output);
        free.push(inside_full);
    }

    void LoanOutFromFull(Tensor **out_full) {
        full.try_pop(out_full);
    }

    void RecycleToFree(Tensor *in_free) {
        free.push(in_free);
    }
};

class TensorPool {
public:
    ~TensorPool();
    BlockingQueuePair *CreateBlockingQueue(std::vector<int> &shape);
    Tensor *CreateTensor(std::vector<int> &shape);
    ITensor *CreateITensor(std::vector<int> &shape);

    void PrintInfo();
    void ExitAllBlockingQueue();

private:
    std::vector<BlockingQueuePair *> bq_pairs_;
    std::vector<Tensor *> tensors_;
    std::vector<ITensor *> itensors_;
};

}  // end of namespace ecas.

#endif // ECAS_CORE_TENSOR_POOL_HPP_