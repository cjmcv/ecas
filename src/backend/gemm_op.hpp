/*!
* \brief Operator. 
*/

#ifndef ECAS_BACKEND_OPERATOR_GEMM_HPP_
#define ECAS_BACKEND_OPERATOR_GEMM_HPP_

#include "operator.hpp"

namespace ecas {

struct GemmKernelParam {
    float alpha = 1.0;
    float beta = 0.0;

    GemmKernelParam& operator=(const GemmKernelParam& in) {
        alpha = in.alpha;
        beta = in.beta;
        return *this;
    }
};

class GemmOp: public Operator {
public:
    static Operator *Creator(std::string &params_str);
    GemmOp(GemmKernelParam &params) :Operator() {
        params_ = params;
    }

    void Help() const;
    bool DimCheck(std::vector<Param> &params, std::vector<ITensor *> &inputs, std::vector<ITensor *> &outputs);
    void Run(std::vector<Param> &params, std::vector<ITensor *> &inputs, std::vector<ITensor *> &outputs);

private:
    GemmKernelParam params_;
};


}  // end of namespace ecas.

#endif // ECAS_BACKEND_OPERATOR_GEMM_HPP_