/*!
* \brief Compute node.
*/

#ifndef ECAS_CORE_NORMAL_NODE_HPP_
#define ECAS_CORE_NORMAL_NODE_HPP_

#include <string>
#include "node.hpp"

namespace ecas {

class NormalNode: public Node {
    
public:
    NormalNode(const std::string &name, Task &&task,
               std::vector<std::vector<int>> &input_shapes,
               std::vector<std::vector<int>> &output_shapes);
    ~NormalNode();

    virtual void Run(std::vector<ITensor *> &input, std::vector<ITensor *> &output) { task_(input, output); }

private:
    Task task_;
};

}  // end of namespace ecas.

#endif //ECAS_CORE_NORMAL_NODE_HPP_