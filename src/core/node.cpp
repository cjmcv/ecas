/*!
* \brief Compute node.
*/

#include "node.hpp"

namespace ecas {

// TODO: 节点间数据衔接
bool Node::CheckInputIsReady() {
    bool is_ready = true;
    for (int i=0; i<inputs_.size(); i++) {
        if (inputs_[i]->full.empty())
            is_ready = false;
    }
    return is_ready;
}

void Node::GetInputsBuffer() {
    
}

}  // end of namespace ecas.