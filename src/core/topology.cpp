/*!
* \brief Topology. 
*        负责构建拓扑关系，不依赖其他模块
*/

#include "topology.hpp"
#include "util/logger.hpp"

namespace dlas {

Topology::Topology() {}

Topology::~Topology() {}

void Topology::Build(std::map<std::string, Node*> &nodes, std::vector<std::vector<std::string>> &&relation) {
    for (int i=0; i<relation.size(); i++) {
        for (int j=1; j<relation[i].size(); j++) {
            printf("%s->%s.\n", relation[i][j-1].c_str(), relation[i][j].c_str());    
        }
    }

    for (int i=0; i<relation.size(); i++) {
        for (int j=1; j<relation[i].size(); j++) {
            std::map<std::string, Node*>::iterator nodes_iter;
            std::map<Node*, std::vector<Node*>>::iterator io_iter;

            // Find the target node.
            Node *target = nullptr;
            nodes_iter = nodes.find(relation[i][j-1].c_str());
            if(nodes_iter != nodes.end())
                target = nodes_iter->second;
            else
                DLAS_LOGE("Can not find node named %s .\n", relation[i][j-1].c_str());
            
            // Find the output node of the target.
            Node *n_out = nullptr;
            nodes_iter = nodes.find(relation[i][j].c_str());
            if(nodes_iter != nodes.end())
                n_out = nodes_iter->second;
            else
                DLAS_LOGE("Can not find node named %s .\n", relation[i][j].c_str());

            // Set output.
            io_iter = output_map_.find(target);
            if(io_iter != output_map_.end()) {
                io_iter->second.push_back(n_out);
            }
            else {
                std::vector<Node*> vec = {n_out};
                output_map_.insert(std::make_pair(target, vec));
            }
            
            // Set Input.
            io_iter = input_map_.find(n_out);
            if(io_iter != input_map_.end()) {
                io_iter->second.push_back(target);
            }
            else {
                std::vector<Node*> vec = {target};
                input_map_.insert(std::make_pair(n_out, vec));
            }
        }
    }
}

std::vector<Node*> *Topology::GetOutputs(Node *node) {
    std::map<Node*, std::vector<Node*>>::iterator io_iter;
    io_iter = output_map_.find(node);
    if (io_iter != output_map_.end())
        return &io_iter->second;
    else
        return nullptr;
}

std::vector<Node*> *Topology::GetInputs(Node *node) {
    std::map<Node*, std::vector<Node*>>::iterator io_iter;
    io_iter = input_map_.find(node);
    if (io_iter != input_map_.end())
        return &io_iter->second;
    else
        return nullptr;
}

}  // end of namespace dlas.