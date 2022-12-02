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
    Node *n = new Node(name);
    n->SetOpTag(op_tag);
    p->nodes.push_back(n);
}

void Session::Run() {
    SessionParams *p = (SessionParams *)params_;
    DLAS_LOGI("Session Running: %s, %d, %d.\n", p->name.c_str(), p->mode, p->num_thread);
}

} // dlas.