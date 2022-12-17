/*!
* \brief Scheduler. 
*        提供节点的调度方案
*/

#ifndef DLAS_CORE_SCHEDULER_HPP_
#define DLAS_CORE_SCHEDULER_HPP_

#include <string>
#include <vector>
#include <map>

#include "node.hpp"
#include "util/logger.hpp"

namespace dlas {

class Scheduler {

public:
    void BuildGroup(std::map<std::string, Node*> &nodes, std::vector<std::vector<std::string>> &&groups);
    void ShowGroups();
private:
    std::vector<std::vector<Node *>> groups_;
};

}  // end of namespace dlas.

#endif // DLAS_CORE_SCHEDULER_HPP_