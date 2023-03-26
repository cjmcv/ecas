/*!
* \brief .
*/

#ifndef ECAS_OPERATOR_LIST_HPP_
#define ECAS_OPERATOR_LIST_HPP_

#include "op_factory.hpp"
// Level 1
#include "dot_op.hpp"
// Level 3
#include "gemm_op.hpp"

namespace ecas {

/////////////////
// Level 1
OPERATOR_REGISTER(dot, DotOp::Creator);

/////////////////
// Level 3
OPERATOR_REGISTER(gemm, GemmOp::Creator);

// void InitOpList() {
// 	OPERATOR_REGISTER(gemm, GemmOp::Creator);
// }

}  // end of namespace ecas.

#endif //ECAS_OPERATOR_LIST_HPP_