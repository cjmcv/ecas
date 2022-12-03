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

#include "util/logger.hpp"
#include "node.hpp"

namespace dlas {

struct SessionParams {
    std::string name;
    ExecutionMode mode;
    int num_thread;
    
    std::vector<Node *> nodes;
};

Session::Session(const std::string &name, SessionConfig &config) {
    SessionParams *p = new SessionParams;
    p->name = name;
    p->mode = config.mode;
    p->num_thread = config.num_thread;

    params_ = (void *)p;
}

Session::~Session() {
    delete (SessionParams *)params_;
}

void Session::CreateNode(const std::string &name, OpTag op_tag) {
    SessionParams *p = (SessionParams *)params_;
    p->nodes.push_back(new Node(name, op_tag));
}

void Session::CreateNode(const std::string &name, Task &&c) {
    SessionParams *p = (SessionParams *)params_;
    p->nodes.push_back(new Node(name, std::forward<Task>(c)));
}

void Session::Run() {
    SessionParams *p = (SessionParams *)params_;
    DLAS_LOGI("Session Running: %s, %d, %d.\n", p->name.c_str(), p->mode, p->num_thread);

    float a = 1.2;
    float b = 2.3;
    for (int i=0; i<p->nodes.size(); i++) {
        p->nodes[i]->Run(&a, &b);
    }
    // TODO: 使用调度器，根据调度器来调度计算节点，含线程开辟；内存planer/内存池；
}

} // dlas.