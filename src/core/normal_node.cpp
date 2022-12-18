/*!
* \brief Compute node.
*/

#include "normal_node.hpp"

namespace ecas {

void TaskB(void *in, void *out) {
    printf("TaskB.\n");
}

NormalNode::NormalNode(const std::string &name, Task &&c) {
    name_ = name;
    task_ = c;  
}

NormalNode::~NormalNode() {

}


}  // end of namespace ecas.