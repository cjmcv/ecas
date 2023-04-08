/*!
* \brief . 
*/

#include "dot_op.hpp"

#include "util/common.hpp"
#include "util/logger.hpp"

namespace ecas {

Operator *DotOp::Creator(std::string &params_str) {
    DotKernelParam params;
    return new DotOp(params);
}

void DotOp::Help() const {
    ECAS_LOGI("Dot: 2 input 1 output");
}

bool DotOp::DimCheck(std::vector<Param> &params, std::vector<ITensor *> &inputs, std::vector<ITensor *> &outputs) {
    if (inputs.size() != 2 || outputs.size() != 1) {
        return false;
    }
    return true;
}

void DotOp::Run(std::vector<Param> &params, std::vector<ITensor *> &inputs, std::vector<ITensor *> &outputs) {
    float *vec_a = (float *)inputs[0]->GetData();
    float *vec_b = (float *)inputs[1]->GetData();
    float *res = (float *)outputs[0]->GetData();
    int len = inputs[0]->shape()[0]; // len

    cpu_dispatcher_->DotKernel(len, vec_a, vec_b, res);
}

}  // end of namespace ecas.