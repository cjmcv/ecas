/*!
* \brief Composit node.
*/

#ifndef DLAS_CORE_COMPOSITE_NODE_HPP_
#define DLAS_CORE_COMPOSITE_NODE_HPP_

#include <string>
#include "node.hpp"

namespace dlas {

class CompositeNode: public Node {
    
public:
    CompositeNode(const std::string &name, std::vector<std::vector<std::string>> &&relation);
    ~CompositeNode();

    virtual void Run(ITensor *input, ITensor *output) { task_(input, output); }

private:
    Task task_;
};

}  // end of namespace dlas.

#endif //DLAS_CORE_COMPOSITE_NODE_HPP_