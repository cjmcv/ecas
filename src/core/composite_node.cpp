/*!
* \brief Composite node.
*/

#include "composite_node.hpp"

namespace dlas {

CompositeNode::CompositeNode(const std::string &name, std::vector<std::vector<std::string>> &&relation) {
    name_ = name;
}

CompositeNode::~CompositeNode() {

}


}  // end of namespace dlas.