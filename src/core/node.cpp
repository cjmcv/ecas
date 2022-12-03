/*!
* \brief Compute node.
*/

#include "node.hpp"

namespace dlas {

void TaskB(void *in, void *out) {
    printf("TaskB.\n");
}

Node::Node(const std::string &name, OpTag tag): name_(name), op_tag_(tag) {
    // TODO: 从工厂拿注册好的函数
    if (op_tag_ == VECTOR_ADD) {
        task_ = std::forward<Task>(TaskB);
    }
}

Node::Node(const std::string &name, Task &&c): name_(name), task_(c) {}

Node::~Node() {

}

}  // end of namespace dlas.