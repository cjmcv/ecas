/*!
* \brief Compute node.
*/

#ifndef DLAS_CORE_NODE_HPP_
#define DLAS_CORE_NODE_HPP_

#include <string>
#include <functional>
#include "dlas/dlas.hpp"

namespace dlas {

// TODO: 内存管理，内存复用在其内部接allocator
class Tensor;
// TODO: 入参，算法参数配置，按类型分组，
// struct Params;
// TODO: 可选择已注册的kernel函数，也可以外设自己的函数

class Node {

    using Task = std::function<void(Tensor *input, Tensor *output)>;

public:
    Node(const std::string &name);
    Node(Task &&c) : task_(c), name_("noname") {}
    ~Node() {}

    void SetOpTag(OpTag op_tag);
    void Run(Tensor *input, Tensor *output) { task_(input, output); }

private:
    Task task_;
    std::string name_;
    OpTag op_tag_;
};

}  // end of namespace dlas.

#endif //DLAS_CORE_NODE_HPP_