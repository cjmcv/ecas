/*!
* \brief Tensor. 
*        数据维度形态等，不涉及具体内存。具体内存Buffer中处理
*/

#ifndef DLAS_CORE_TENSOR_HPP_
#define DLAS_CORE_TENSOR_HPP_

#include <string>
#include <vector>

#include "buffer.hpp"

namespace dlas {

class Tensor {

public:
    Tensor();
    ~Tensor();

private:
    // dimension.
    std::vector<int> shape_;
    Buffer *buffer_;
};

}  // end of namespace dlas.

#endif // DLAS_CORE_TENSOR_HPP_