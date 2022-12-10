/*!
* \brief Compute node.
*/

#include "normal_node.hpp"

namespace dlas {

void TaskB(void *in, void *out) {
    printf("TaskB.\n");
}

NormalNode::NormalNode(const std::string &name, OpTag tag) {
    name_ = name;
    op_tag_ = tag;
    // TODO: 从工厂拿注册好的函数
    if (op_tag_ == VECTOR_ADD) {
        task_ = std::forward<Task>(TaskB);
    }
}

NormalNode::NormalNode(const std::string &name, Task &&c) {
    name_ = name;
    task_ = c;  
}

NormalNode::~NormalNode() {

}


}  // end of namespace dlas.