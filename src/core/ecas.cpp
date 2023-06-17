/*!
* \brief Session. 
*        最外层管理类，提供接口。
*/

#include "ecas/ecas.hpp"

#include "async_graph.hpp"
#include "operator_executor.hpp"

#include "util/logger.hpp"
#include "util/common.hpp"
#include "util/timer.hpp"

#include "kernel/generic/funcs_list.hpp"

namespace ecas {

/////////////////////////////////////////////
// ITensor
/////////////////////////////////////////////


/////////////////////////////////////////////
// Session
/////////////////////////////////////////////

struct SessionParams {
    Allocator *allocator; // graph是否要独占一个TensorPool？topology能否给tensorpool提供依赖信息？    
    OperatorExecutor *executor;  //      
    AsyncGraph *graph;
};

Session::Session(const std::string &name, SessionConfig &config) {
    SessionParams *p = new SessionParams;
    p->executor = new OperatorExecutor();
    p->allocator = new Allocator();
    p->graph = new AsyncGraph(name, config.mode, config.num_thread, p->allocator);
    
    params_ = (void *)p;
}

Session::~Session() {
    SessionParams *p = (SessionParams *)params_;
    delete p->executor;
    delete p->graph;
    delete p->allocator;
    delete p;
}

///////////
// Memory
ITensor *Session::CreateITensor(std::vector<int> &&shape, DataType type, void *data) {
    SessionParams *p = (SessionParams *)params_;
    Tensor *t = p->allocator->CreateTensor(shape, type, data);
    return t;
}

/////////////////////
// Operator executor
void *Session::CreateOp(std::string op_name, std::string op_params) {
    SessionParams *p = (SessionParams *)params_;
    return p->executor->CreateOp(op_name, op_params);
}

void Session::OpRun(void *op_ptr, std::vector<Param> &params, 
                    std::vector<ITensor *> &inputs, std::vector<ITensor *> &outputs) {
    SessionParams *p = (SessionParams *)params_;
    p->executor->OpRun(op_ptr, params, inputs, outputs);
}

//////////////
// AsyncGraph
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

void Session::Start(void *usr) {
    SessionParams *p = (SessionParams *)params_;
    p->graph->Start(usr);   
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

//////////////
// UtilBox
struct UtilBoxParams {
    bool is_timer_close;
    std::vector<util::Timer *> timers;
};
UtilBox::UtilBox() {
    params_ = new UtilBoxParams;
}
UtilBox::~UtilBox() {
    UtilBoxParams *p = (UtilBoxParams *)params_;
    // Timer.
    for (uint32_t i=0; i<p->timers.size(); i++) {
        delete p->timers[i];
    }
    delete p;
}

// UtilBox::Timer.
void *UtilBox::GetNewTimer(std::string name, uint32_t num) {
    UtilBoxParams *p = (UtilBoxParams *)params_;
    util::Timer *t = new util::Timer(name, num);
    p->timers.push_back(t);
    return t;
}

void UtilBox::TimerStart(void *timer_handle) {
    util::Timer *t = (util::Timer *)timer_handle;
    t->Start();
}

void UtilBox::TimerStop(void *timer_handle, uint32_t idx, uint32_t print_interval) {
    util::Timer *t = (util::Timer *)timer_handle;
    t->Stop(idx, print_interval);
}

//////////////
// Math
float Math::expf(float x) {
    return ecas_expf(x);
}

float Math::sqrtf(float x) {
    return ecas_sqrtf(x);
}

} // ecas.