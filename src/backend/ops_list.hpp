/*!
* \brief .
*/

#ifndef ECAS_OPERATOR_LIST_HPP_
#define ECAS_OPERATOR_LIST_HPP_

#include "op_factory.hpp"
#include "gemm_op.hpp"

namespace ecas {

OPERATOR_REGISTER(gemm, GemmOp::Creator);

// void InitOpList() {
// 	OPERATOR_REGISTER(gemm, GemmOp::Creator);
// }

}  // end of namespace ecas.

#endif //ECAS_OPERATOR_LIST_HPP_