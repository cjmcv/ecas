/*!
* \brief Compute node.
*/

#ifndef DLAS_CORE_NODE_HPP_
#define DLAS_CORE_NODE_HPP_

#include <string>
#include "dlas/dlas.hpp"

namespace dlas {

// TODO: 内存管理，内存复用在其内部接allocator
class Tensor;
// TODO: 入参，算法参数配置，按类型分组，
// struct Params;
// TODO: 可选择已注册的kernel函数，也可以外设自己的函数

class Node {
public:
    virtual ~Node() {};
    virtual void Run(void *input, void *output) = 0;

protected:
    std::string name_;
    OpTag op_tag_;
};

}  // end of namespace dlas.

#endif //DLAS_CORE_NODE_HPP_