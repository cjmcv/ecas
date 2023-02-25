/*!
* \brief Compute node.
*/

#include "normal_node.hpp"

namespace ecas {

void TaskB(void *in, void *out) {
    printf("TaskB.\n");
}

NormalNode::NormalNode(const std::string &name, Task &&task,
                       std::vector<std::vector<int>> &input_dims, 
                       std::vector<std::vector<int>> &output_dims) {
    name_ = name;
    task_ = task;

    input_dims_ = input_dims;
    output_dims_ = output_dims;
}

NormalNode::~NormalNode() {

}


}  // end of namespace ecas.