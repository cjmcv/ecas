/*!
* \brief Compute node.
*/

#include "node.hpp"

namespace dlas {

Node::Node(const std::string &name): name_(name) {

}

void Node::SetOpTag(OpTag op_tag) {
    op_tag_ = op_tag;
}

}  // end of namespace dlas.