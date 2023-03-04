/*!
* \brief Operator. 
*/

#ifndef ECAS_BACKEND_OPERATOR_GEMM_HPP_
#define ECAS_BACKEND_OPERATOR_GEMM_HPP_

#include "operator.hpp"

namespace ecas {

class GemmOp: public Operator {
public:
    void Run(std::vector<ITensor *> &inputs, std::vector<Param> &params, std::vector<ITensor *> &outputs);
};

}  // end of namespace ecas.

#endif // ECAS_BACKEND_OPERATOR_GEMM_HPP_