/*!
* \brief Scheduler. 
*        提供节点的调度方案, 多线程管理，包含多线程间数据内存交互
*/

#ifndef ECAS_CORE_SCHEDULER_HPP_
#define ECAS_CORE_SCHEDULER_HPP_

#include <string>
#include <vector>
#include <thread>
#include <map>

#include "node.hpp"
#include "tensor_pool.hpp"
#include "util/blocking_queue.hpp"
#include "util/logger.hpp"

namespace ecas {

class Scheduler {

public:
    Scheduler();
    ~Scheduler();

    void MarkGroupId(Node *node, int group_id);
    void UpdateGroups();
    
    ////////////////////////
    /// Tensors management
    bool CheckShapes();
    // Setup tensors for cross node interaction.
    void SetupTensors();

    ////////////////////////
    /// Serial Execution
    // Breadth First.
    void BuildBfsPass(Node *input_node) {}
    void BfsExecute(Node *input_node, ITensor *input_data);

    ////////////////////////
    /// Parallel execution
    // Group nodes, and each group uses one thread.
    void BuildGroup(std::map<std::string, Node*> &nodes, 
                    std::vector<std::vector<std::string>> &&groups);
    void ShowGroups();
    inline int group_size() { return groups_.size(); }
    // inline std::vector<std::vector<Node *>> &group_nodes() { return groups_; };
    void TasksSpawn();
    void TasksStop();
    void TasksJoin();

private:
    /// Serial Execution
    std::vector<Node *> bfs_nodes_;

    /// Parallel execution
    // groups_[group_id][node_ptr]
    std::vector<std::vector<Node *>> groups_;
    std::vector<std::vector<Node *>> groups_temp_;

    std::vector<std::thread> threads_;
    bool is_stop_;

    // //
    TensorPool *tensor_pool_;
    bool is_tensor_setup_;
};

}  // end of namespace ecas.

#endif // ECAS_CORE_SCHEDULER_HPP_