/*!
* \brief 
*/

#include "operator_executor.hpp"

namespace ecas {

OperatorExecutor::OperatorExecutor() {}

OperatorExecutor::~OperatorExecutor() {
    for (int i=0; i<ops_.size(); i++) {
        delete ops_[i];
    }
}

Operator *OperatorExecutor::CreateOp(std::string &op_name, std::string &op_params) {
    Operator *op = OpFactory::GetInstance().CreateOpByName(op_name, op_params);
    ops_.push_back(op);
    return op;
}

void OperatorExecutor::OpRun(void *op_ptr, std::vector<Param> &params, 
                             std::vector<ITensor *> &inputs, std::vector<ITensor *> &outputs) {
    Operator *op = (Operator *)op_ptr;
    op->Run(params, inputs, outputs);
    
    // std::map<std::string, Operator*>::iterator iter;
    // iter = op_map_.find(op_name);
    // if (iter != op_map_.end()) {
    //     iter->second->Run(params, inputs, outputs);
    // }
    // else {
    //     Operator *op = new GemmOp(); // TODO: 工厂创建
    //     op_map_.insert(std::make_pair(op_name, op));
    //     op->Run(params, inputs, outputs);
    // }
    // for(iter = op_map_.begin(); iter != op_map_.end(); iter++) {

    // }
}

}  // end of namespace ecas.
