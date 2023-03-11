/*!
* \brief . 
*/

#include "gemm_op.hpp"

namespace ecas {

bool GemmOp::DimCheck(std::vector<Param> &params, std::vector<ITensor *> &inputs, std::vector<ITensor *> &outputs) {
    if (inputs.size() != 2 || outputs.size() != 1) {
        return false;
    }
    return true;
}

void GemmOp::Run(std::vector<Param> &params, std::vector<ITensor *> &inputs, std::vector<ITensor *> &outputs) {
    float *A = (float *)inputs[0]->data;
    float *B = (float *)inputs[1]->data;
    float *C = (float *)outputs[0]->data;
    int M = outputs[0]->shape[0]; // h
    int N = outputs[0]->shape[1]; // W
    int K = inputs[0]->shape[1];
    float alpha = params[0].fval;

    printf("GemmOp::Run: %d, %d, %d, %d, %d.\n", inputs.size(), outputs.size(), M, N, K);
    // cpu_dispatcher_->GemmKernel(M, N, K, alpha, A, K, B, N, C, N);
}

}  // end of namespace ecas.