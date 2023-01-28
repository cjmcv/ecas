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

// TODO: gpu数据持有
// 基本数据计算与操作
class Tensor {

public:
    Tensor(std::vector<int> &shape);
    ~Tensor();

    void CloneTo(Tensor *out);

private:
    int id_;
    int size_;
    std::vector<int> shape_;
    Buffer *buffer_;
};

}  // end of namespace ecas.

#endif // ECAS_CORE_TENSOR_HPP_