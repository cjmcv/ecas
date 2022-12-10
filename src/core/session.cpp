/*!
* \brief Session. 
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
#include "tensor.hpp"

namespace dlas {

struct SessionParams {
    std::string name;
    ExecutionMode mode;
    int num_thread;
    
    // std::vector<Node *> nodes;
    std::vector<Tensor *> inputs;
    std::vector<Tensor *> outputs;

    std::map<std::string, Node*> nodes; // 包含普通节点和组合节点
};

Session::Session(const std::string &name, SessionConfig &config) {
    SessionParams *p = new SessionParams;
    p->name = name;
    p->mode = config.mode;
    p->num_thread = config.num_thread;

    p->nodes.clear();
    p->inputs.clear();
    p->outputs.clear();
    params_ = (void *)p;
}

Session::~Session() {
    SessionParams *p = new SessionParams;

    for(std::map<std::string, Node*>::iterator iter = p->nodes.begin(); 
        iter != p->nodes.end(); iter++) {
        delete iter->second;
    }
    p->nodes.clear(); 
    // malloc_trim(0)

    for (int i = 0; i < p->inputs.size(); i++) {
        delete p->inputs[i];
    }
    p->inputs.clear();
    std::vector<Tensor *>().swap(p->inputs);
    //
    for (int i = 0; i < p->outputs.size(); i++) {
        delete p->outputs[i];
    }
    p->outputs.clear();
    std::vector<Tensor *>().swap(p->outputs);

    delete (SessionParams *)params_;
}

// rvalue references
void Session::CreateInput(std::vector<unsigned int> &&shape) {
    SessionParams *p = (SessionParams *)params_;
    p->inputs.push_back(new Tensor(shape));
    printf("in1 shape: %d.\n", shape.size());
}

void Session::CreateInput(std::vector<std::vector<unsigned int>> &&shapes) {
    SessionParams *p = (SessionParams *)params_;
    for (int i=0; i<shapes.size(); i++) {
        printf("in2 shape: %d.\n", shapes[i].size());
        p->inputs.push_back(new Tensor(shapes[i]));        
    }
}

void Session::CreateOutput(std::vector<unsigned int> &&shape) {
    SessionParams *p = (SessionParams *)params_;
    p->outputs.push_back(new Tensor(shape));
    printf("out1 shape: %d.\n", shape.size());
}

void Session::CreateOutput(std::vector<std::vector<unsigned int>> &&shapes) {
    SessionParams *p = (SessionParams *)params_;
    for (int i=0; i<shapes.size(); i++) {
        printf("out2 shape: %d.\n", shapes[i].size());
        p->outputs.push_back(new Tensor(shapes[i]));        
    }
}

void Session::CreateNode(const std::string &name, OpTag op_tag) {
    SessionParams *p = (SessionParams *)params_;
    p->nodes.insert(std::make_pair(name, new NormalNode(name, op_tag)));
}
void Session::CreateNode(const std::string &name, Task &&task) {
    SessionParams *p = (SessionParams *)params_;
    p->nodes.insert(std::make_pair(name, new NormalNode(name, std::forward<Task>(task))));
}

void Session::Run(const std::string &name) {
    SessionParams *p = (SessionParams *)params_;
    DLAS_LOGI("Session Running: %s, %d, %d.\n", p->name.c_str(), p->mode, p->num_thread);

    // for (int i=0; i<p->nodes.size(); i++) {
    //     if (p->nodes[i].name() == name) {

    //     }
    // }


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