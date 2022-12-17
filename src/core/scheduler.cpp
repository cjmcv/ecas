/*!
* \brief Scheduler. 
*/

#include "scheduler.hpp"

namespace dlas {

void Scheduler::BuildGroup(std::map<std::string, Node *> &nodes, std::vector<std::vector<std::string>> &&groups) {
    groups_.resize(groups.size());
    for (int i = 0; i < groups.size(); i++) {
        groups_[i].resize(groups[i].size());
        for (int j = 0; j < groups[i].size(); j++) {
            std::map<std::string, Node *>::iterator iter = nodes.find(groups[i][j]);
            if (iter != nodes.end()) {
                groups_[i][j] = iter->second;
            }
            else {
                DLAS_LOGI("BuildGroup -> Can not find node named %s .\n", groups[i][j]);
            }
        }
    }
}
    
void Scheduler::ShowGroups() {
    for (int i = 0; i < groups_.size(); i++) {
        DLAS_LOGS("group: %d, ", i);
        for (int j = 0; j < groups_[i].size(); j++) {
            DLAS_LOGS("%d, ", groups_[i][j]);
        }
        DLAS_LOGS("\n");
    }
}

}  // end of namespace dlas.