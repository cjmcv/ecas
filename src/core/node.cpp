/*!
* \brief Compute node.
*/

#include "node.hpp"
#include "util/logger.hpp"

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

bool Node::BorrowIo(std::vector<ITensor *> &inputs, std::vector<ITensor *> &outputs) {
    input_tensors_.clear();
    // printf("input_queues_.size: %d.\n", input_queues_.size());
    for (int i=0; i<input_queues_.size(); i++) {
        Tensor *inside_full;
        // printf("input_queues_[%d]->full.size : %d.\n", i, input_queues_[i]->full.size());
        bool is_ready = input_queues_[i]->full.wait_and_pop(&inside_full);
        if (!is_ready) return false;
        input_tensors_.push_back(inside_full);
    }
    output_tensors_.clear();
    // printf("output_queues_.size: %d.\n", output_queues_.size());
    for (int i=0; i<output_queues_.size(); i++) {
        Tensor *inside_free;
        // printf("output_queues_[%d]->free.size : %d.\n", i, output_queues_[i]->free.size());
        bool is_ready = output_queues_[i]->free.wait_and_pop(&inside_free);
        if (!is_ready) return false;
        output_tensors_.push_back(inside_free);
    }
    // Get ITensor
    inputs.clear();
    for (int i=0; i<input_tensors_.size(); i++) {
        ITensor *it = input_tensors_[i]->GetITensorPtr();
        inputs.push_back(it);
    }
    outputs.clear();
    for (int i=0; i<output_tensors_.size(); i++) {
        ITensor *it = output_tensors_[i]->GetITensorPtr();
        outputs.push_back(it);
    }
    // Check id
    for (int i=1; i<inputs.size(); i++) {
        if (inputs[i]->id != inputs[0]->id) {
            ECAS_LOGE("Node::BorrowIo -> The ID of Tensor in the same group is inconsistent.\n");
        }
    }
    // Pass id
    for (int i=0; i<outputs.size(); i++) {
        outputs[i]->id = inputs[0]->id;
    }
    return true;
}

void Node::RecycleIo() {
    // TODO: 按需进行异步的跨设备内存拷贝。
    for (int i=0; i<input_queues_.size(); i++) {
        input_queues_[i]->free.push(input_tensors_[i]);
    }
    for (int i=0; i<output_queues_.size(); i++) {
        output_queues_[i]->full.push(output_tensors_[i]);
    }
}

}  // end of namespace ecas.