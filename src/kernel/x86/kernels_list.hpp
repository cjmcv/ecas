/*!
* \brief .
*/

#ifndef ECAS_KERNELS_LIST_HPP_
#define ECAS_KERNELS_LIST_HPP_

#include <string>
#include <vector>
#include <map>

namespace ecas {

void Dot(int len, const float *vec_a, const float *vec_b, float *res);

void Gemm(const int M, const int N, const int K, 
		  const float ALPHA,
		  const float *A, const int lda,
		  const float *B, const int ldb,
		  float *C, const int ldc);

}  // end of namespace ecas.

#endif //ECAS_KERNELS_LIST_HPP_