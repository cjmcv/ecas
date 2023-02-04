/*!
* \brief Compute node.
*/

#include "node.hpp"

namespace ecas {

//
void Node::SwapQueueOrder(std::vector<BlockingQueuePair *> &queues, int i, int j) {
    BlockingQueuePair *temp = queues[i];
    queues[i] = queues[j];
    queues[j] = temp;
}

void Node::ReorderInputQueues() {
    // Make the order of the input queues consistent with the order of the input nodes
    if (input_nodes_ != nullptr) {
        for (int ni = 0; ni < input_nodes_->size(); ni++) {
            std::string target_name = (*input_nodes_)[ni]->name();
            for (int qi = 0; qi < input_queues_.size(); qi++) {
                if (target_name == input_queues_[qi]->front_name) {
                    if (ni == qi) 
                        continue;
                    else 
                        SwapQueueOrder(input_queues_, ni, qi);
                }
            }
        }
    }
}

void Node::ReorderOutputQueues() {
    // Make the order of the output queues consistent with the order of the output nodes
    if (output_nodes_ != nullptr) {
        for (int ni = 0; ni < output_nodes_->size(); ni++) {
            std::string target_name = (*output_nodes_)[ni]->name();
            for (int qi = 0; qi < output_queues_.size(); qi++) {
                if (target_name == output_queues_[qi]->rear_name) {
                    if (ni == qi)
                        continue;
                    else
                        SwapQueueOrder(output_queues_, ni, qi);
                }
            }
        }
    }
}

// TODO: 节点间数据衔接
bool Node::CheckIoIsReady() {
    bool is_ready = true;
    for (int i=0; i<input_queues_.size(); i++) {
        if (input_queues_[i]->full.empty())
            is_ready = false;
    }
    for (int i=0; i<output_queues_.size(); i++) {
        if (output_queues_[i]->free.empty())
            is_ready = false;
    }
    return is_ready;
}

void Node::GetInputs(std::vector<ITensor *> &inputs) {
    inputs.clear();
    for (int i=0; i<input_queues_.size(); i++) {
        if (input_queues_[i]->full.empty()) {
            
        }
    }

    //
        // std::vector<Node *> *ins = n->input_nodes();
        // if (ins != nullptr) {
        //     for(int i=0; i<ins->size(); i++) {
        //         ECAS_LOGS("%s", (*ins)[i]->name().c_str());
        //         if (i != ins->size() - 1) ECAS_LOGS(", ");
        //     }
        // }
}

void Node::GetOutputs(std::vector<ITensor *> &outputs) {
    
}

}  // end of namespace ecas.