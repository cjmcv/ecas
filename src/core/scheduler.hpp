/*!
* \brief Scheduler. 
*        提供节点的调度方案
*/

#ifndef ECAS_CORE_SCHEDULER_HPP_
#define ECAS_CORE_SCHEDULER_HPP_

#include <string>
#include <vector>
#include <thread>
#include <map>

#include "node.hpp"
#include "util/blocking_queue.hpp"
#include "util/logger.hpp"

namespace ecas {

class Scheduler {

public:
    Scheduler();
    ~Scheduler() {};

    ////////////////////////
    /// Serial Execution
    // Breadth First.
    void BuildBfsPass(Node *input_node) {}
    void BfsExecute(Node *input_node, ITensor *input_data);

    ////////////////////////
    /// Parallel execution
    void SetupIoBuffer();
    // Group nodes, and each group uses one thread.
    void BuildGroup(std::map<std::string, Node*> &nodes, 
                    std::vector<std::vector<std::string>> &&groups);
    void ShowGroups();
    // inline std::vector<std::vector<Node *>> &group_nodes() { return groups_; };
    void TasksSpawn();
    void TasksStop();
    void TasksJoin();

private:
    /// Serial Execution
    std::vector<Node *> bfs_nodes_;

    /// Parallel execution
    std::vector<std::vector<Node *>> groups_;
    std::vector<std::thread> threads_;
    bool is_stop_;

    // //
    // BlockingQueue<Tensor *> outs_free_;
    // BlockingQueue<Tensor *> *outs_full_;
};

}  // end of namespace ecas.

#endif // ECAS_CORE_SCHEDULER_HPP_