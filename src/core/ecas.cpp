/*!
* \brief Session. 
*        最外层管理类，提供接口。
*/

#include "ecas/ecas.hpp"

#include "async_graph.hpp"
#include "tensor_pool.hpp"

namespace ecas {

struct SessionParams {
    AsyncGraph *graph;
    TensorPool *tensor_pool_; // graph是否要独占一个TensorPool？topology能否给tensorpool提供依赖信息？
};

Session::Session(const std::string &name, SessionConfig &config) {
    SessionParams *p = new SessionParams;
    p->tensor_pool_ = new TensorPool();
    p->graph = new AsyncGraph(name, config.mode, config.num_thread, p->tensor_pool_);
    params_ = (void *)p;
}

Session::~Session() {
    SessionParams *p = (SessionParams *)params_;
    delete p->graph;
    delete p->tensor_pool_;
    delete p;
}

void Session::CreateNode(const std::string &name, Task &&task, 
                         std::vector<std::vector<int>> &&input_shapes, 
                         std::vector<std::vector<int>> &&output_shapes, 
                         int group_id) {
    SessionParams *p = (SessionParams *)params_;                        
    p->graph->CreateNode(name, std::forward<Task>(task), 
                         std::forward<std::vector<std::vector<int>>>(input_shapes), 
                         std::forward<std::vector<std::vector<int>>>(output_shapes), 
                         group_id);
}

void Session::CreateNode(const std::string &name, std::vector<std::vector<std::string>> &&relation) {
    SessionParams *p = (SessionParams *)params_;
    p->graph->CreateNode(name, std::forward<std::vector<std::vector<std::string>>>(relation));
}

void Session::BuildGraph(std::vector<std::vector<std::string>> &&relation) {
    SessionParams *p = (SessionParams *)params_;
    p->graph->BuildGraph(std::forward<std::vector<std::vector<std::string>>>(relation));
}

void Session::ShowInfo() {
    SessionParams *p = (SessionParams *)params_;
    p->graph->ShowInfo();
}

void Session::Start() {
    SessionParams *p = (SessionParams *)params_;
    p->graph->Start();   
}

void Session::Stop() {        
    SessionParams *p = (SessionParams *)params_;
    p->graph->Stop(); 
}

void Session::GraphFeed(ITensor *in) {
    SessionParams *p = (SessionParams *)params_;
    p->graph->Feed(in); 
}

void Session::GraphGetResult(ITensor *out) {
    SessionParams *p = (SessionParams *)params_;
    p->graph->GetResult(out); 
}

//

ITensor *Session::CreateITensor(std::vector<int> &&shape) {
    SessionParams *p = (SessionParams *)params_;
    return p->tensor_pool_->CreateITensor(shape);
}

} // ecas.