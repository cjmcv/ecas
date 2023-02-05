/*!
* \brief Scheduler. 
*/

#include "scheduler.hpp"

#include <queue>
#include <chrono>

namespace ecas {

Scheduler::Scheduler() {
    is_stop_ = false;
    is_profiler_start_ = false;
    groups_.clear();
    tensor_pool_ = new TensorPool;
}

Scheduler::~Scheduler() {
    delete tensor_pool_;
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

////////////////////////
/// Tensors Setup
void Scheduler::SetupIoTensors(Node *input_node, Node *output_node) {
    if (input_node == nullptr || output_node == nullptr)
        ECAS_LOGE("SetupIoTensors -> Both input and output nodes must exist.\n");
    if (input_node->input_shapes().size() != 1 || output_node->output_shapes().size() != 1)
        ECAS_LOGE("SetupIoTensors -> Input node has one input, output node has one output.\n");

    BlockingQueuePair *bqp;
    bqp = tensor_pool_->CreateBlockingQueue(input_node->input_shapes()[0]);
    bqp->front_name = "input";
    bqp->rear_name = input_node->name();
    input_node->AppendInputs(bqp);

    bqp = tensor_pool_->CreateBlockingQueue(output_node->output_shapes()[0]);
    bqp->front_name = output_node->name();
    bqp->rear_name = "output";
    output_node->AppendOutputs(bqp);
}

void Scheduler::SetupInteractTensors() {
    // Only nodes in the group are valid
    for (int i = 0; i < groups_.size(); i++) {
        for (int j = 0; j < groups_[i].size(); j++) {
            Node *n = (Node *)groups_[i][j];
            std::vector<std::vector<int>> input_shapes = n->input_shapes();
            std::vector<Node *> *input_nodes = n->input_nodes();

            // if input_nodes == nullptr, it indicates that it is an input node of the graph 
            // and does not need to check the output shape.
            if (input_nodes == nullptr) {
                continue;
            }

            // The number of input shapes and input nodes should be the same.
            if (input_nodes->size() != input_shapes.size())
                ECAS_LOGE("SetupInteractTensors -> output_nodes->size() != output_shapes.size(): %d vs %d.\n",
                          input_nodes->size(), input_shapes.size());

            // Check each of input nodes.
            for (int si = 0; si < input_nodes->size(); si++) {
                Node *in_node = (*input_nodes)[si];
                std::vector<std::vector<int>> need_match_shapes = in_node->output_shapes();

                if (need_match_shapes.size() == 1) {
                    if (need_match_shapes[0] != input_shapes[si]) {
                        ECAS_LOGE("SetupInteractTensors -> Shape check failed (node %s to %s).\n",
                                  n->name().c_str(), in_node->name().c_str());
                    }
                }
                else if (need_match_shapes.size() > 1) {
                    // TODO: 目前其中一个能匹配就算匹配上了，但实际上看可能会在a+b=>c时，a和b的输出维度一致和c的其中一个输入吻合，c的另一个不吻合，则应该是不匹配的。但目前的策略是会判断为匹配的。
                    bool is_pass = false;
                    for (int ni = 0; ni < need_match_shapes.size(); ni++) {
                        if (need_match_shapes[ni] == input_shapes[si])
                            is_pass = true;
                    }
                    if (is_pass == false) {
                        ECAS_LOGE("SetupInteractTensors -> Shape check failed (node %s to %s).\n",
                                  n->name().c_str(), in_node->name().c_str());
                    }
                }
                else {
                    ECAS_LOGE("SetupInteractTensors -> Shape check failed: need_match_shapes.size() <= 0 \
                                  (node %s to %s).\n",
                              n->name().c_str(), in_node->name().c_str());
                }
                // Check passed and allocate BlockingQueuePair.
                BlockingQueuePair *bqp = tensor_pool_->CreateBlockingQueue(input_shapes[si]);
                bqp->front_name = in_node->name();
                bqp->rear_name = n->name();
                in_node->AppendOutputs(bqp);
                n->AppendInputs(bqp);
            }
        }
    }
}

void Scheduler::ReorderTensors() {
    for (int i = 0; i < groups_.size(); i++) {
        for (int j = 0; j < groups_[i].size(); j++) {
            Node *n = (Node *)groups_[i][j];
            n->ReorderInputQueues();
            n->ReorderOutputQueues();
        }
    }
}

void Scheduler::SetupTensors(Node *input_node, Node *output_node) {
    SetupInteractTensors();
    SetupIoTensors(input_node, output_node);    
    ReorderTensors();
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

void Scheduler::TasksSpawn() {
    if (groups_.size() == 0) {
        ECAS_LOGE("TasksSpawn -> groups_.size() == 0, please call function BuildGraph first.\n");
    }

    std::vector<std::vector<Node *>> &groups = groups_;
    for (unsigned i = 0; i < groups.size(); ++i) {
        threads_.emplace_back([this, i, groups]() -> void {
            std::vector<ITensor *> inputs;
            std::vector<ITensor *> outputs;

            while (!is_stop_) {
                // 同一组的按顺序堵塞执行，不会有帧差
                for (int ni=0; ni<groups[i].size(); ni++) {
                    Node *n = groups[i][ni];
                    printf("Start BorrowIo: %s.\n", n->name().c_str());
                    n->BorrowIo(inputs, outputs);
                    printf("%s -> (%d, %d).\n", n->name().c_str(), inputs.size(), outputs.size());
                    n->Run(inputs, outputs);
                } 
            }
            printf("is_stop_: %d.\n", is_stop_);
        });
    }
}

void Scheduler::TasksStop() {
    is_stop_ = true;
    // ECAS_LOGI("TasksStop.\n");
}

void Scheduler::TasksJoin() {
    for (auto& t : threads_) {
      t.join();
    }
}

////////////////////////
/// Profiler
void Scheduler::StartProfile() {
    if (is_profiler_start_ == true)
        return;

    is_profiler_start_ = true;
    std::thread *t = new std::thread([this]() -> void {
        while (is_profiler_start_ == true) {
            tensor_pool_->PrintInfo();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });
}

void Scheduler::EndProfile() {
    is_profiler_start_ = false;
}

}  // end of namespace ecas.