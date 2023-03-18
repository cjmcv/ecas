/*!
* \brief 异步图最外层，主要包含节点组图，线程调度管理和内存管理三大块。
*/

#ifndef ECAS_CORE_ASYNC_GRAPH_HPP_
#define ECAS_CORE_ASYNC_GRAPH_HPP_

#include "normal_node.hpp"
#include "composite_node.hpp"
#include "tensor.hpp"
#include "topology.hpp"
#include "scheduler.hpp"

namespace ecas {

class AsyncGraph {
public:
    AsyncGraph(const std::string &name, ExecutionMode mode, int num_thread, TensorPool *tensor_pool);
    ~AsyncGraph();

    void CreateNode(const std::string &name, Task &&task, 
                    std::vector<std::vector<int>> &&in_shapes, 
                    std::vector<std::vector<int>> &&out_shapes,
                    int group_id = 0);
    void CreateNode(const std::string &name, std::vector<std::vector<std::string>> &&relation);
    void BuildGraph(std::vector<std::vector<std::string>> &&relation);
    void ShowInfo();

    void Start(void *usr);
    void Stop();

    // Asynchronous function.
    void Feed(ITensor *in);
    // Get the result after calling the Feed.
    void GetResult(ITensor *out);

private:
    // Check whether the shapes match and create tensors for node interaction.
    void SetupInteractTensors();
    void SetupIoTensors();
    void ReorderTensors();
    //
    void StartProfile();
    void EndProfile();

private:
    std::string name_;
    ExecutionMode mode_;
    int num_thread_;

    std::map<std::string, Node*> nodes_; // 包含普通节点和组合节点
    Node *input_node_;
    Node *output_node_;
    std::vector<Node *> graph_nodes_; // 参与组建图的节点

    Topology topo_;
    void *usr_;
    Scheduler scheduler_;
    TensorPool *tensor_pool_;

    bool is_profiler_start_;
};

}  // end of namespace ecas.

#endif //ECAS_CORE_ASYNC_GRAPH_HPP_