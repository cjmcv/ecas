/*!
* \brief AsyncGraph. 
*        最外层管理类，提供接口。
*/

// TODO: 对外提供内存管理/复用
//       对外提供多线程任务调度，开线程，ringbuffer/blocking_queue交互
//       对外提供的单函数，通过session实例进行绑定，session统一资源管理
//       

#include "async_graph.hpp"

#include <iostream>
#include <vector>
#include <map>

#include "util/logger.hpp"

namespace ecas {

AsyncGraph::AsyncGraph(const std::string &name, ExecutionMode mode, int num_thread, TensorPool *tensor_pool) {
    name_ = name;
    mode_ = mode;
    num_thread_ = num_thread;

    name_ = "noname";
    num_thread_ = 1;
    nodes_.clear();

    input_node_ = nullptr;
    output_node_ = nullptr;

    tensor_pool_ = tensor_pool;
}

AsyncGraph::~AsyncGraph() {
    for (std::map<std::string, Node *>::iterator iter = nodes_.begin();
        iter != nodes_.end(); iter++) {
        delete iter->second;
    }
    nodes_.clear();
}

void AsyncGraph::CreateNode(const std::string &name, Task &&task, 
                            std::vector<std::vector<int>> &&input_dims,
                            std::vector<std::vector<int>> &&output_dims,
                            int group_id) {
    Node *n = new NormalNode(name, std::forward<Task>(task), input_dims, output_dims);

    scheduler_.MarkGroupId(n, group_id);
    nodes_.insert(std::make_pair(name, n));
}

void AsyncGraph::CreateNode(const std::string &name, std::vector<std::vector<std::string>> &&relation) {

    CompositeNode *node = new CompositeNode(name, std::forward<std::vector<std::vector<std::string>>>(relation));
    nodes_.insert(std::make_pair(name, node));
}

void AsyncGraph::SetupInteractTensors() {
    for (int i = 0; i < graph_nodes_.size(); i++) {
        Node *n = graph_nodes_[i];
        std::vector<std::vector<int>> input_dims = n->input_dims();
        std::vector<Node *> *input_nodes = n->input_nodes();

        // if input_nodes == nullptr, it indicates that it is an input node of the graph
        // and does not need to check the output shape.
        if (input_nodes == nullptr) {
            continue;
        }

        // The number of input shapes and input nodes should be the same.
        if (input_nodes->size() != input_dims.size())
            ECAS_LOGE("SetupInteractTensors -> output_nodes->size() != output_dims.size(): %d vs %d.\n",
                      input_nodes->size(), input_dims.size());

        // Check each of input nodes.
        for (int si = 0; si < input_nodes->size(); si++) {
            Node *in_node = (*input_nodes)[si];
            std::vector<std::vector<int>> need_match_dims = in_node->output_dims();

            if (need_match_dims.size() == 1) {
                if (need_match_dims[0] != input_dims[si]) {
                    ECAS_LOGE("SetupInteractTensors -> Shape check failed (node %s to %s).\n",
                              n->name().c_str(), in_node->name().c_str());
                }
            }
            else if (need_match_dims.size() > 1) {
                // TODO: 目前其中一个能匹配就算匹配上了，但实际上看可能会在a+b=>c时，a和b的输出维度一致和c的其中一个输入吻合，c的另一个不吻合，则应该是不匹配的。但目前的策略是会判断为匹配的。
                bool is_pass = false;
                for (int ni = 0; ni < need_match_dims.size(); ni++) {
                    if (need_match_dims[ni] == input_dims[si])
                        is_pass = true;
                }
                if (is_pass == false) {
                    ECAS_LOGE("SetupInteractTensors -> Shape check failed (node %s to %s).\n",
                              n->name().c_str(), in_node->name().c_str());
                }
            }
            else {
                ECAS_LOGE("SetupInteractTensors -> Shape check failed: need_match_dims.size() <= 0 \
                          (node %s to %s).\n", n->name().c_str(), in_node->name().c_str());
            }
            // Check passed and allocate BlockingQueuePair.
            std::vector<int> tensor_shapes;
            tensor_shapes.assign(input_dims[si].begin() + 1, input_dims[si].end());
            BlockingQueuePair *bqp = tensor_pool_->CreateBlockingQueue(tensor_shapes, (DataType)input_dims[si][0]);
            bqp->front_name = in_node->name();
            bqp->rear_name = n->name();
            in_node->AppendOutputs(bqp);
            n->AppendInputs(bqp);
        }
    }
}

void AsyncGraph::SetupIoTensors() {
    if (input_node_ == nullptr || output_node_ == nullptr)
        ECAS_LOGE("SetupIoTensors -> Both input and output nodes must exist.\n");
    if (input_node_->input_dims().size() != 1 || output_node_->output_dims().size() != 1)
        ECAS_LOGE("SetupIoTensors -> Input node has one input, output node has one output.\n");

    std::vector<int> tensor_shapes;
    BlockingQueuePair *bqp;

    // Skip data type saved in shape[0].
    tensor_shapes.assign(input_node_->input_dims()[0].begin() + 1, input_node_->input_dims()[0].end());
    bqp = tensor_pool_->CreateBlockingQueue(tensor_shapes, (DataType)input_node_->input_dims()[0][0]);
    bqp->front_name = "input";
    bqp->rear_name = input_node_->name();
    input_node_->AppendInputs(bqp);

    tensor_shapes.assign(output_node_->output_dims()[0].begin() + 1, output_node_->output_dims()[0].end());
    bqp = tensor_pool_->CreateBlockingQueue(tensor_shapes, (DataType)output_node_->output_dims()[0][0]);
    bqp->front_name = output_node_->name();
    bqp->rear_name = "output";
    output_node_->AppendOutputs(bqp);
}

void AsyncGraph::ReorderTensors() {
    for (int i = 0; i < graph_nodes_.size(); i++) {
        graph_nodes_[i]->ReorderInputQueues();
        graph_nodes_[i]->ReorderOutputQueues();
    }
}

void AsyncGraph::BuildGraph(void *usr, std::vector<std::vector<std::string>> &&relation) {
    usr_ = usr;
    // Build topology
    topo_.Build(nodes_, std::forward<std::vector<std::vector<std::string>>>(relation));
    // Specify inputs and outputs for each node according to the constructed topology.
    std::map<std::string, Node*>::iterator iter;
    for(iter = nodes_.begin(); iter != nodes_.end(); iter++) {
        std::vector<Node*> *inputs = topo_.GetInputs(iter->second);
        iter->second->SetInputNodes(inputs);
        std::vector<Node*> *outputs = topo_.GetOutputs(iter->second);
        iter->second->SetOutputNodes(outputs);
    }

    // Find the graph IO nodes according to the number of inputs and outputs.
    // Input node of the graph: no input.
    // Output node of the graph: no output.
    // Only one input node and one output node are allowed
    // Nodes with neither input nor output are not included in the graph.
    for(iter = nodes_.begin(); iter != nodes_.end(); iter++) {
        if (iter->second->input_nodes() == nullptr && iter->second->output_nodes() == nullptr) {
            // independent, not included in the graph
        }
        else if (iter->second->input_nodes() == nullptr) {
            if (input_node_ != nullptr) 
                ECAS_LOGE("BuildGraph -> Only one input node is allowed.\n");
            input_node_ = iter->second;
        }
        else if (iter->second->output_nodes() == nullptr) {
            if (output_node_ != nullptr) 
                ECAS_LOGE("BuildGraph -> Only one output node is allowed.\n");
            output_node_ = iter->second;
        }
    }

    // Group nodes.
    scheduler_.UpdateGroups();
    scheduler_.GetGraphNodes(graph_nodes_);

    // Check shape && allocate memory && reorder
    SetupInteractTensors();
    SetupIoTensors();
    ReorderTensors();
    ECAS_LOGI("Finish AsyncGraph::BuildGraph.\n");
}

//// It will determine the thread allocation scheme.
// void AsyncGraph::Group(std::vector<std::vector<std::string>> &&groups) {
//     AsyncGraphParams *p = (AsyncGraphParams *)params_;
//     scheduler_.BuildGroup(nodes_, std::forward<std::vector<std::vector<std::string>>>(groups));
// }

void AsyncGraph::ShowInfo() {
    ECAS_LOGS("\n>>>>>>>>> AsyncGraph ShowInfo >>>>>>>>>\n");
    ECAS_LOGS("AsyncGraph: %s.\n", name_.c_str());
    ECAS_LOGS("Input node: %s.\n", input_node_->name().c_str());
    ECAS_LOGS("Output node: %s.\n", output_node_->name().c_str());

    std::map<std::string, Node*>::iterator iter;
    for(iter = nodes_.begin(); iter != nodes_.end(); iter++) {
        Node *n = iter->second;
        std::vector<Node *> *ins = n->input_nodes();
        std::vector<Node *> *outs = n->output_nodes();
        ECAS_LOGS("node: %s (%d) -> in: [", n->name().c_str(), n);
        for (int i = 0; i<n->input_dims().size(); i++) {
            ECAS_LOGS("%d(", n->input_dims()[i][0]);
            for (int j=1; j<n->input_dims()[i].size(); j++) {
                ECAS_LOGS("%d", n->input_dims()[i][j]);
                if (j != n->input_dims()[i].size() - 1) ECAS_LOGS(",");
            }
            ECAS_LOGS(")");
            if (i != n->input_dims().size() - 1) ECAS_LOGS(",");
        }
        ECAS_LOGS("], out: [");
        for (int i = 0; i<n->output_dims().size(); i++) {
            ECAS_LOGS("%d(", n->output_dims()[i][0]);
            for (int j=1; j<n->output_dims()[i].size(); j++) {
                ECAS_LOGS("%d", n->output_dims()[i][j]);
                if (j != n->output_dims()[i].size() - 1) ECAS_LOGS(",");
            }
            ECAS_LOGS(")");
            if (i != n->output_dims().size() - 1) ECAS_LOGS(",");
        }
        ECAS_LOGS("]\n");
    }
    ECAS_LOGS("\n");
    //
    ECAS_LOGS("Node Relationship: \n");
    for(iter = nodes_.begin(); iter != nodes_.end(); iter++) {
        Node *n = iter->second;
        std::vector<Node *> *ins = n->input_nodes();
        std::vector<Node *> *outs = n->output_nodes();
        ECAS_LOGS("%s -> in: [", n->name().c_str());
        if (ins != nullptr) {
            for(int i=0; i<ins->size(); i++) {
                ECAS_LOGS("%s", (*ins)[i]->name().c_str());
                if (i != ins->size() - 1) ECAS_LOGS(", ");
            }
        }
        ECAS_LOGS("], out: [");
        if (outs != nullptr) {
            for(int i=0; i<outs->size(); i++) {
                ECAS_LOGS("%s", (*outs)[i]->name().c_str());
                if (i != outs->size() - 1) ECAS_LOGS(", ");
            }
        }
        ECAS_LOGS("].\n");
    }
    ECAS_LOGS("\n");
    //
    ECAS_LOGS("Tensors: \n");
    for(iter = nodes_.begin(); iter != nodes_.end(); iter++) {
        Node *n = iter->second;
        std::vector<BlockingQueuePair *> ins = n->input_queues();
        std::vector<BlockingQueuePair *> outs = n->output_queues();
        if (ins.size() == 0 && outs.size() == 0)
            continue;
        ECAS_LOGS("%s -> in: [", n->name().c_str());
        for (int i = 0; i < ins.size(); i++) {
            ECAS_LOGS("%d(%s)", ins[i], ins[i]->front_name.c_str());
            if (i != ins.size() - 1)
                ECAS_LOGS(", ");
        }
        ECAS_LOGS("], out: [");
        for (int i = 0; i < outs.size(); i++) {
            ECAS_LOGS("%d(%s)", outs[i], outs[i]->rear_name.c_str());
            if (i != outs.size() - 1)
                ECAS_LOGS(", ");
        }
        ECAS_LOGS("].\n");
    }

    ECAS_LOGS("\n");
    scheduler_.ShowGroups();
    ECAS_LOGS(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\n");
}

void AsyncGraph::StartProfile() {
    if (is_profiler_start_ == true)
        return;

    is_profiler_start_ = true;
    std::thread *t = new std::thread([this]() -> void {
        while (is_profiler_start_ == true) {
            tensor_pool_->PrintInfo();
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    });
}

void AsyncGraph::EndProfile() {
    is_profiler_start_ = false;
}

void AsyncGraph::Start() {
    // Start all task threads.
    scheduler_.TasksSpawn(usr_);
    // StartProfile();      
}

void AsyncGraph::Stop() {        
    // Stop all task threads.
    scheduler_.TasksStop(tensor_pool_);
    scheduler_.TasksJoin();
    // EndProfile();
    ECAS_LOGI("AsyncGraph::Stop().\n");
}

void AsyncGraph::Feed(ITensor *in) {
    // ECAS_LOGI("AsyncGraph Running: %s, %d, %d.\n", name_.c_str(), p->mode, p->num_thread);
    input_node_->input_queues()[0]->Enqueue(in);
    // scheduler_.BfsExecute(input_node_, &in);
}

void AsyncGraph::GetResult(ITensor *out) {
    output_node_->output_queues()[0]->Dequeue(out);
}

} // ecas.