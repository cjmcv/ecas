/*!
* \brief Tensor. 
*        数据维度形态等，不涉及具体内存。具体内存Buffer中处理
*/

#ifndef ECAS_CORE_TENSOR_HPP_
#define ECAS_CORE_TENSOR_HPP_

#include <string>
#include <vector>

#include "buffer.hpp"
#include "ecas/ecas.hpp"

namespace ecas {

// ITensor 为Tensor的父类，对外数据交互, 隐藏细节，不允许外部构造
// Tensor 为ITensor子类，实际内存操作者，跨设备，异步拷贝等
// Buffer 实际内存管理提供者，多设备多类型内存
// Tensor绑定Buffer，而一般不持有Buffer, 除非使用外部内存，外部使用对Buffer无感。

// 基本数据计算与操作
class Tensor : public ITensor {

public:
    Tensor(std::vector<int> &shape, DataType type);
    // Tensor(ITensor &in);
    ~Tensor();

    inline uint32_t size() { return size_; }
    void BindBuffer(Buffer *buffer);
    void CopyFrom(ITensor *in);
    void CopyTo(ITensor *out);
    
    // Use external memory.
    void BindHostDataPtr(void *data);
    void *GetData(MemoryMode mode = ON_HOST);
    void Print();

private:
    void CheckDimension(ITensor *target);

private:
    uint32_t size_;
    bool is_owned_buffer_; // 只能持有不含内存的host buffer(即由外部引入指针)，其他包含内存的buffer均不持有
    Buffer *buffer_;
};

}  // end of namespace ecas.

#endif // ECAS_CORE_TENSOR_HPP_