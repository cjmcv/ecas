/*!
* \brief Tensor. 
*        数据维度形态等，不涉及具体内存。具体内存Buffer中处理
*/

#ifndef ECAS_CORE_TENSOR_HPP_
#define ECAS_CORE_TENSOR_HPP_

#include <string>
#include <vector>

#include "buffer.hpp"

namespace ecas {

// 基本数据计算与操作
class Tensor {

public:
    Tensor(std::vector<unsigned int> &shape);
    ~Tensor();

private:
    // dimension.
    int size_;
    std::vector<unsigned int> shape_;
    Buffer *buffer_;
};

}  // end of namespace ecas.

#endif // ECAS_CORE_TENSOR_HPP_