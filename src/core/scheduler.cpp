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
}

Scheduler::~Scheduler() {
}

// TODO: 可以有缺省id号，在未设置id号时，自动给其新增一个只有它自己的group。
//       默认每个节点都各开一个线程，分组主要目的是为了绑核。
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

void Scheduler::GetGraphNodes(std::vector<Node *> &graph_nodes) {
    graph_nodes.clear();
    for (int i = 0; i < groups_.size(); i++) {
        for (int j = 0; j < groups_[i].size(); j++) {
            graph_nodes.push_back((Node *)groups_[i][j]);
        }
    }
}

////////////////////////
/// Serial Execution
// TODO: 目前无法处理有环的情况
void Scheduler::BfsExecute(Node *input_node, ITensor *input_data) {
    std::queue<Node *> tasks;
    tasks.push(input_node);
    while (!tasks.empty()) {
        Node *t = tasks.front();
        // ITensor *input;
        // ITensor *output;
        // t->Run(input, output);
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
                ECAS_LOGI("BuildGroup -> Can not find node named %s .\n", groups[i][j].c_str());
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
            ECAS_LOGS("%s", ((Node *)groups_[i][j])->name().c_str()); // groups_[i][j]
            if (j != groups_[i].size() - 1) ECAS_LOGS(", ");
        }
        ECAS_LOGS("\n");
    }
}

void Scheduler::TasksSpawn(void *usr) {
    if (groups_.size() == 0) {
        ECAS_LOGE("TasksSpawn -> groups_.size() == 0, please call function BuildGraph first.\n");
    }

    std::vector<std::vector<Node *>> &groups = groups_;
    printf("group size: %d.\n", groups_.size());
    for (unsigned i = 0; i < groups.size(); ++i) {
        threads_.emplace_back([this, i, groups, usr]() -> void {
            std::vector<ITensor *> inputs;
            std::vector<ITensor *> outputs;
            while (!is_stop_) {
                // 同一组的按顺序堵塞执行，不会有帧差
                for (int ni=0; ni<groups[i].size(); ni++) {
                    Node *n = groups[i][ni];
                    // printf("Start BorrowIo: %s.\n", n->name().c_str());
                    bool ret = n->BorrowIo(inputs, outputs);
                    if (ret == false) break;
                    // printf("%s -> (%d, %d).\n", n->name().c_str(), inputs.size(), outputs.size());
                    n->Run(usr, inputs, outputs);
                    n->RecycleIo();
                }
            }
            ECAS_LOGI("is_stop_: %d.\n", is_stop_);
        });
    }
    ECAS_LOGI("Scheduler::TasksSpawn End.\n");
}

void Scheduler::TasksStop(TensorPool *pool) {
    is_stop_ = true;
    pool->ExitAllBlockingQueue();
}

void Scheduler::TasksJoin() {
    for (auto& t : threads_) {
        t.join();
    }
}

}  // end of namespace ecas.