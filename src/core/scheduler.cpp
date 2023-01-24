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

void Scheduler::MarkGroupId(Node *node, int group_id) {
    static int init_size = 10;
    if (groups_temp_.size() != init_size) {
        groups_temp_.resize(init_size);
    }
    if (group_id > groups_temp_.size()) {
        ECAS_LOGE("group_id should be smaller than %d.\n", groups_temp_.size());
    }
    groups_temp_[group_id].push_back(node);
}

void Scheduler::UpdateGroups() {
    groups_.clear();
    for (int i=0; i<groups_temp_.size(); i++) {
        if (groups_temp_[i].size() == 0)
            continue;
        groups_.push_back(groups_temp_[i]);
    }
}

////////////////////////
/// Tensors management
bool Scheduler::CheckShapes() {
    // Check whether the shapes match.
    // 注意：一节点每个输入必须各对应一个节点，每个输出也必须各对应一个节点。
    // std::vector<std::vector<int>> out_shapes = target.output_shapes();
    // n_out.input_shapes();
    for (int i = 0; i < groups_.size(); i++) {
        for (int j = 0; j < groups_[i].size(); j++) {
            Node *n = (Node *)groups_[i][j];
            std::vector<std::vector<int>> output_shapes = n->output_shapes();
            std::vector<Node *> *output_nodes = n->output_nodes();

            if (output_nodes != nullptr && output_nodes->size() != output_shapes.size())
                ECAS_LOGE("CheckShapes -> output_nodes->size() != output_shapes.size(): %d vs %d.\n", 
                          output_nodes->size(), output_shapes.size());

            for (int si=0; si<output_shapes.size(); si++) {
                // output_shapes[si];
            }
        }
        ECAS_LOGS("\n");
    }
    return true;
}

void Scheduler::SetupTensors() {
    CheckShapes();
    if (groups_.size() <= 1) {
        // Single thread.
        // The output of the node and the input of its output node 
        // use the same Tensor.

        // Check shapes.
        
        // tensor_pool_.CreateTensor();
    }
    else {
        // Multiple threads.
        // The output of the node and the input of its output node 
        // use the same BlockingQueue.
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
    // groups_[group_id][node_ptr]
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
    ECAS_LOGS("Groups: \n");
    for (int i = 0; i < groups_.size(); i++) {
        if (groups_[i].size() == 0)
            ECAS_LOGE("ShowGroups -> groups_[%d].size() == 0.\n", i)

        ECAS_LOGS("%d -> ", i);
        for (int j = 0; j < groups_[i].size(); j++) {
            ECAS_LOGS("%s (%d)", ((Node *)groups_[i][j])->name().c_str(), groups_[i][j]);
            if (j != groups_[i].size() - 1) ECAS_LOGS(", ");
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