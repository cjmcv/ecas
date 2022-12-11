/*!
* \brief Topology.
*/

#ifndef DLAS_CORE_TOPOLOGY_NODE_HPP_
#define DLAS_CORE_TOPOLOGY_NODE_HPP_

#include <string>
#include <vector>
#include <map>

namespace dlas {

class Node;

class Topology {    
public:
    Topology();
    ~Topology();
    void Build(std::map<std::string, Node*> &nodes, std::vector<std::vector<std::string>> &&relation);

    std::vector<Node*> *GetOutputs(Node *node);
    std::vector<Node*> *GetInputs(Node *node);

    void Show();

private:
    std::map<Node*, std::vector<Node*>> output_map_;
    std::map<Node*, std::vector<Node*>> input_map_;
};

}  // end of namespace dlas.

#endif //DLAS_CORE_TOPOLOGY_NODE_HPP_