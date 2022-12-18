/*!
* \brief Scheduler. 
*        提供节点的调度方案
*/

#ifndef ECAS_CORE_SCHEDULER_HPP_
#define ECAS_CORE_SCHEDULER_HPP_

#include <string>
#include <vector>
#include <map>

#include "node.hpp"
#include "util/logger.hpp"

namespace ecas {

class Scheduler {

public:    
    /// Serial Execution
    // Breadth First.
    void BfsExecute(Node *input_node, ITensor *input_data);

    /// Parallel execution
    // Group nodes, and each group uses one thread.
    void BuildGroup(std::map<std::string, Node*> &nodes, 
                    std::vector<std::vector<std::string>> &&groups);
    void ShowGroups();

private:
    /// Serial Execution
    std::vector<Node *> bfs_nodes_;

    /// Parallel execution
    std::vector<std::vector<Node *>> groups_;
};

}  // end of namespace ecas.

#endif // ECAS_CORE_SCHEDULER_HPP_