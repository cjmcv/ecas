/*!
* \brief Session. 
*        最外层管理类，提供接口。
*/

#include "ecas/ecas.hpp"

#include "async_graph.hpp"
#include "tensor_pool.hpp"
#include "operator_executor.hpp"

namespace ecas {

struct SessionParams {
    TensorPool *tensor_pool; // graph是否要独占一个TensorPool？topology能否给tensorpool提供依赖信息？    
    AsyncGraph *graph;
};

Session::Session(const std::string &name, SessionConfig &config) {
    SessionParams *p = new SessionParams;
    p->tensor_pool = new TensorPool();
    p->graph = new AsyncGraph(name, config.mode, config.num_thread, p->tensor_pool);

    params_ = (void *)p;
}

Session::~Session() {
    SessionParams *p = (SessionParams *)params_;
    delete p->graph;
    delete p->tensor_pool;
    delete p;
}

void Session::CreateNode(const std::string &name, Task &&task, 
                         std::vector<std::vector<int>> &&input_dims, 
                         std::vector<std::vector<int>> &&output_dims, 
                         int group_id) {
    SessionParams *p = (SessionParams *)params_;                        
    p->graph->CreateNode(name, std::forward<Task>(task), 
                         std::forward<std::vector<std::vector<int>>>(input_dims), 
                         std::forward<std::vector<std::vector<int>>>(output_dims), 
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

ITensor *Session::CreateITensor(std::vector<int> &&shape, DataType type) {
    SessionParams *p = (SessionParams *)params_;
    Tensor *t = p->tensor_pool->CreateTensor(shape, type);
    return t->GetITensorPtr();
}

//

void Session::OpRun(std::string op_name, std::vector<Param> &params, 
                    std::vector<ITensor *> &inputs, std::vector<ITensor *> &outputs) {
    OperatorExecutor::GetInstance().OpRun(op_name, params, inputs, outputs);
}

} // ecas.