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

// ITensor 对外数据交互
// Tensor 各种内存操作
// Buffer 实际内存管理提供者，多设备多类型内存

// 基本数据计算与操作
class Tensor {

public:
    Tensor(std::vector<int> &shape);
    // Tensor(ITensor &in);
    ~Tensor();

    inline ITensor *GetITensorPtr() { return it_; }

    void CopyFrom(ITensor &in);
    void CopyTo(ITensor *out);

private:
    int size_;
    ITensor *it_;
    Buffer *buffer_;
};

}  // end of namespace ecas.

#endif // ECAS_CORE_TENSOR_HPP_