/*!
* \brief 
*/

#include "op_factory.hpp"

#include "util/logger.hpp"
#include "ops_list.hpp"

namespace ecas {

Operator *OpFactory::CreateOpByName(std::string &op_name, std::string &param_str) {
    std::map<std::string, OpCreator>::iterator it = op_creator_map_.find(op_name);
    if (it == op_creator_map_.end()) {
        ECAS_LOGE("Can not find Op: %s.\n Registered Op: < %s>",
                  op_name.c_str(), PrintList().c_str());
        return nullptr;
    }

    OpCreator creator = it->second;
    if (!creator)
        return nullptr;

    return creator(param_str);
}

void OpFactory::RegisterOpClass(std::string &op_name, OpCreator creator) {
    if (op_creator_map_.count(op_name) != 0) {
        ECAS_LOGW("Op name: %s has already been registered.", op_name.c_str());
        return;
    }
    op_creator_map_[op_name] = creator;
}

std::string OpFactory::PrintList() {
    std::string out = "";
    std::map<std::string, OpCreator>::iterator it = op_creator_map_.begin();
    while (it != op_creator_map_.end()) {
        out += it->first;
        out += " ";
        it++;
    }
    return out;
}

}  // end of namespace ecas.
