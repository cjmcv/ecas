/*!
* \brief 
*/

#include "operator_executor.hpp"

#include "backend/gemm_op.hpp"

namespace ecas {

void OperatorExecutor::OpRun(void *op_ptr, std::vector<Param> &params, 
                             std::vector<ITensor *> &inputs, std::vector<ITensor *> &outputs) {
    std::map<std::string, Operator*>::iterator iter;
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
