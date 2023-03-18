/*!
* \brief . 
*/

#include "gemm_op.hpp"

#include "util/common.hpp"
#include "util/logger.hpp"

namespace ecas {

Operator *GemmOp::Creator(std::string &params_str) {
    GemmKernelParam params;
    // TODO: atoi 
    params.alpha = atof(util::StrProcessor::FetchSubStr(params_str, "alpha:", ",").c_str());
    params.beta = atof(util::StrProcessor::FetchSubStr(params_str, "beta:", ",").c_str());
    ECAS_LOGI("Create GemmOp, params.alpha: %.3f, params.beta: %.3f.\n", params.alpha, params.beta);
    return new GemmOp(params);
}

void GemmOp::Help() const {
    ECAS_LOGI("Gemm: 2 input 1 output      \
               Params example: alpha: 1.0, beta: 2.0");
}

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

    // printf("GemmOp::Run: %d, %d, %d, %d, %d.\n", inputs.size(), outputs.size(), M, N, K);
    // cpu_dispatcher_->GemmKernel(M, N, K, alpha, A, K, B, N, C, N);
}

}  // end of namespace ecas.