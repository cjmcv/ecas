/*!
* \brief Composit node.
*/

#ifndef ECAS_CORE_COMPOSITE_NODE_HPP_
#define ECAS_CORE_COMPOSITE_NODE_HPP_

#include <string>
#include "node.hpp"

namespace ecas {

class CompositeNode: public Node {
    
public:
    CompositeNode(const std::string &name, std::vector<std::vector<std::string>> &&relation);
    ~CompositeNode();

    virtual void Run(ITensor *input, ITensor *output) { task_(input, output); }

private:
    Task task_;
};

}  // end of namespace ecas.

#endif //ECAS_CORE_COMPOSITE_NODE_HPP_