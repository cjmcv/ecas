/*!
* \brief Compute node.
*/

#ifndef DLAS_CORE_NORMAL_NODE_HPP_
#define DLAS_CORE_NORMAL_NODE_HPP_

#include <string>
#include "node.hpp"

namespace dlas {

class NormalNode: public Node {
    
public:
    NormalNode(const std::string &name, Task &&c);
    ~NormalNode();

    virtual void Run(ITensor *input, ITensor *output) { task_(input, output); }

private:
    Task task_;
};

}  // end of namespace dlas.

#endif //DLAS_CORE_NORMAL_NODE_HPP_