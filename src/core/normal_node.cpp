/*!
* \brief Compute node.
*/

#include "normal_node.hpp"

namespace ecas {

void TaskB(void *in, void *out) {
    printf("TaskB.\n");
}

NormalNode::NormalNode(const std::string &name, Task &&task,
                       std::vector<std::vector<int>> &input_shapes,
                       std::vector<std::vector<int>> &output_shapes) {
    name_ = name;
    task_ = task;

    input_shapes_ = input_shapes;
    output_shapes_ = output_shapes;
}

NormalNode::~NormalNode() {

}


}  // end of namespace ecas.