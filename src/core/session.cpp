/*!
* \brief Session. 
*        最外层管理类，提供接口。
*/

// TODO: 对外提供内存管理/复用
//       对外提供多线程任务调度，开线程，ringbuffer/blocking_queue交互
//       对外提供的单函数，通过session实例进行绑定，session统一资源管理
//       

#include "dlas/dlas.hpp"

#include <iostream>
#include <vector>
#include <map>

#include "util/logger.hpp"
#include "normal_node.hpp"
#include "composite_node.hpp"
#include "tensor.hpp"
#include "topology.hpp"

namespace dlas {

struct SessionParams {
    std::string name;
    ExecutionMode mode;
    int num_thread;
    
    std::vector<Tensor *> inputs;
    std::vector<Tensor *> outputs;

    std::map<std::string, Node*> nodes; // 包含普通节点和组合节点
    std::vector<Node *> input_nodes;
    std::vector<Node *> output_nodes;

    Topology topo;

    SessionParams() {
        name = "noname";
        num_thread = 1;
        nodes.clear();
        inputs.clear();
        outputs.clear();
    }
    ~SessionParams() {
        for(std::map<std::string, Node*>::iterator iter = nodes.begin(); 
            iter != nodes.end(); iter++) {
            delete iter->second;
        }
        nodes.clear(); 
        // malloc_trim(0)

        for (int i = 0; i < inputs.size(); i++) {
            delete inputs[i];
        }
        inputs.clear();
        std::vector<Tensor *>().swap(inputs);
        //
        for (int i = 0; i < outputs.size(); i++) {
            delete outputs[i];
        }
        outputs.clear();
        std::vector<Tensor *>().swap(outputs);
    }
};

Session::Session(const std::string &name, SessionConfig &config) {
    SessionParams *p = new SessionParams;
    p->name = name;
    p->mode = config.mode;
    p->num_thread = config.num_thread;
    params_ = (void *)p;
}

Session::~Session() {
    SessionParams *p = new SessionParams;
    delete (SessionParams *)params_;
}

void Session::CreateNode(const std::string &name, OpTag op_tag) {
    SessionParams *p = (SessionParams *)params_;
    p->nodes.insert(std::make_pair(name, new NormalNode(name, op_tag)));
}
void Session::CreateNode(const std::string &name, Task &&task) {
    SessionParams *p = (SessionParams *)params_;
    p->nodes.insert(std::make_pair(name, new NormalNode(name, std::forward<Task>(task))));
}
void Session::CreateNode(const std::string &name, std::vector<std::vector<std::string>> &&relation) {
    SessionParams *p = (SessionParams *)params_;
    CompositeNode *node = new CompositeNode(name, std::forward<std::vector<std::vector<std::string>>>(relation));
    p->nodes.insert(std::make_pair(name, node));
}

void Session::BuildGraph(std::vector<std::vector<std::string>> &&relation) {
    SessionParams *p = (SessionParams *)params_;
    // Build topology
    p->topo.Build(p->nodes, std::forward<std::vector<std::vector<std::string>>>(relation));
    // Specify inputs and outputs for each node according to the constructed topology.
    std::map<std::string, Node*>::iterator iter;
    for(iter = p->nodes.begin(); iter != p->nodes.end(); iter++) {
        std::vector<Node*> *inputs = p->topo.GetInputs(iter->second);
        iter->second->SetInputNodes(inputs);
        std::vector<Node*> *outputs = p->topo.GetOutputs(iter->second);
        iter->second->SetOutputNodes(outputs);
    }
    // Find the graph IO nodes according to the number of inputs and outputs.
    // Input node of the graph: no input.
    // Output node of the graph: no output.
    // Nodes with neither input nor output are not included in the graph.
    p->input_nodes.clear();
    p->output_nodes.clear();
    for(iter = p->nodes.begin(); iter != p->nodes.end(); iter++) {
        if (iter->second->input_nodes() == nullptr && iter->second->output_nodes() == nullptr) {
            // independent, not included in the graph
        }
        else if (iter->second->input_nodes() == nullptr) {
            p->input_nodes.push_back(iter->second);
        }
        else if (iter->second->output_nodes() == nullptr) {
            p->output_nodes.push_back(iter->second);
        }
    }
}

void Session::ShowInfo() {
    SessionParams *p = (SessionParams *)params_;

    DLAS_LOGS("\n>>>>>>>>> Session ShowInfo >>>>>>>>>\n");
    DLAS_LOGS("Session: %s.\n", p->name.c_str());
    DLAS_LOGS("Input nodes: ");
    for (int i=0; i<p->input_nodes.size(); i++) {
        DLAS_LOGS("%s", p->input_nodes[i]->name().c_str());
        if (i != p->input_nodes.size() - 1) DLAS_LOGS(", ");
    }
    DLAS_LOGS("\n");
    DLAS_LOGS("Output nodes: ");
    for (int i=0; i<p->output_nodes.size(); i++) {
        DLAS_LOGS("%s", p->output_nodes[i]->name().c_str());
        if (i != p->output_nodes.size() - 1) DLAS_LOGS(", ");
    }
    DLAS_LOGS("\n");

    std::map<std::string, Node*>::iterator iter;
    for(iter = p->nodes.begin(); iter != p->nodes.end(); iter++) {
        Node *n = iter->second;
        std::vector<Node *> *ins = n->input_nodes();
        std::vector<Node *> *outs = n->output_nodes();
        DLAS_LOGS("node: %s -> in: [", n->name().c_str());
        if (ins != nullptr) {
            for(int i=0; i<ins->size(); i++) {
                DLAS_LOGS("%s", (*ins)[i]->name().c_str());
                if (i != ins->size() - 1) DLAS_LOGS(", ");
            }
        }
        DLAS_LOGS("], out: [");
        if (outs != nullptr) {
            for(int i=0; i<outs->size(); i++) {
                DLAS_LOGS("%s", (*outs)[i]->name().c_str());
                if (i != outs->size() - 1) DLAS_LOGS(", ");
            }
        }
        DLAS_LOGS("].\n");
    }
    DLAS_LOGS(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\n");
}

void Session::Run(const std::string &name) {
    SessionParams *p = (SessionParams *)params_;
    DLAS_LOGI("Session Running: %s, %d, %d.\n", p->name.c_str(), p->mode, p->num_thread);

    float a = 1.2;
    float b = 2.3;
    std::map<std::string, Node*>::iterator iter = p->nodes.find(name);
    if(iter != p->nodes.end()){
        iter->second->Run(&a, &b);
    }
    else{
        DLAS_LOGI("Can not find node named %s .\n", name);
    }

    // TODO: 使用调度器，根据调度器来调度计算节点，含线程开辟；内存planer/内存池；
}

} // dlas.