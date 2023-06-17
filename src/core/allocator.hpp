/*!
* \brief Allocator. 
*/

#ifndef ECAS_CORE_TENSOR_POOL_HPP_
#define ECAS_CORE_TENSOR_POOL_HPP_

#include "tensor.hpp"
#include "buffer.hpp"
#include "util/blocking_queue.hpp"

/*
        Allocator
        |        |
     Tensor <—— Buffer
*/
namespace ecas {

// TODO: 兼并同步模式，不使用blocking_queue. 提供统一对外的结构体
// TODO: tensor和buffer平级，tensor可以和不同buffer绑定。
//       buffer有size的概念，tensor有shape，只要size大于tensor所需size，即可绑定
//       Tensor不共享，但如两个Tensor无依赖关系，则可以公用一个buffer，达到节省资源的目的
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

class Allocator {
public:
    ~Allocator();
    BlockingQueuePair *CreateBlockingQueue(std::vector<int> &shape, DataType type);
    Tensor *CreateTensor(std::vector<int> &shape, DataType type, void *data);

    void PrintInfo();
    void ExitAllBlockingQueue();

private:
    Buffer *CreateBuffer(MemoryType type, uint32_t size);

    std::vector<BlockingQueuePair *> bq_pairs_; // 用于节点间数据交互
    std::vector<Tensor *> tensors_; // TODO: 添加Itensor与tensor映射，可通过Itensor找回tensor。
    std::vector<Buffer *> buffers_;
};

}  // end of namespace ecas.

#endif // ECAS_CORE_TENSOR_POOL_HPP_