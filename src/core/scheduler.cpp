/*!
* \brief Scheduler. 
*/

#include "scheduler.hpp"

#include <queue>
#include <chrono>

namespace ecas {

Scheduler::Scheduler() {
    is_stop_ = false;
    groups_.clear();
    tensor_pool_ = new TensorPool;
    is_tensor_setup_ = false;
}

Scheduler::~Scheduler() {
    delete tensor_pool_;
    is_tensor_setup_ = false;
}

////////////////////////
/// Tensors management
void Scheduler::SetupTensors() {
    if (groups_.size() <= 1) {
        // Single thread.
        // 节点输出和其输出节点的输入用同一个Tensor
    }
    else {
        // Multiple threads.
        // 节点输出和其输出节点的输入用同一组BlockingQueue
    }
    is_tensor_setup_ = true;
}

////////////////////////
/// Serial Execution
void Scheduler::BfsExecute(Node *input_node, ITensor *input_data) {
    if (!is_tensor_setup_) {
        ECAS_LOGE("TasksSpawn -> please call function SetupTensors first.\n");
    }
    std::queue<Node *> tasks;
    tasks.push(input_node);
    while (!tasks.empty()) {
        Node *t = tasks.front();
        ITensor *input;
        ITensor *output;
        t->Run(input, output);
        std::vector<Node *> *outs = t->output_nodes();
        if (outs != nullptr) {
            for (unsigned int i=0; i<outs->size(); i++) {
                tasks.push((*outs)[i]);
            }            
        }
        tasks.pop();
    }
}

////////////////////////
/// Parallel execution
void Scheduler::BuildGroup(std::map<std::string, Node *> &nodes, 
                           std::vector<std::vector<std::string>> &&groups) {
    groups_.resize(groups.size());
    for (int i = 0; i < groups.size(); i++) {
        groups_[i].resize(groups[i].size());
        for (int j = 0; j < groups[i].size(); j++) {
            std::map<std::string, Node *>::iterator iter = nodes.find(groups[i][j]);
            if (iter != nodes.end()) {
                groups_[i][j] = iter->second;
            }
            else {
                ECAS_LOGI("BuildGroup -> Can not find node named %s .\n", groups[i][j]);
            }
        }
    }
}
    
void Scheduler::ShowGroups() {
    for (int i = 0; i < groups_.size(); i++) {
        ECAS_LOGS("group: %d, ", i);
        for (int j = 0; j < groups_[i].size(); j++) {
            ECAS_LOGS("%d, ", groups_[i][j]);
        }
        ECAS_LOGS("\n");
    }
}

void Scheduler::TasksSpawn() {
    if (groups_.size() == 0) {
        ECAS_LOGE("TasksSpawn -> groups_.size() == 0, please call function BuildGraph first.\n");
    }
    if (!is_tensor_setup_) {
        ECAS_LOGE("TasksSpawn -> please call function SetupTensors first.\n");
    }

    std::vector<std::vector<Node *>> &groups = groups_;
    for (unsigned i = 0; i < groups.size(); ++i) {
        threads_.emplace_back([this, i, groups]() -> void {
            while (!is_stop_) {
                for (int ni=0; ni<groups[i].size(); ni++) {
                    printf("ni: %s, tid: %d.\n", groups[i][ni]->name().c_str(), std::this_thread::get_id());
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    // // Wait and check intputs from depends.
                    // bool is_pass = WaitCheckInputs(node);
                    // if (!is_pass) { break; }
                    // Process(node, timer);                    
                } 
            } 
        });
    }
}

void Scheduler::TasksStop() {
    is_stop_ = true;
}

void Scheduler::TasksJoin() {
    for (auto& t : threads_) {
      t.join();
    }
}

}  // end of namespace ecas.