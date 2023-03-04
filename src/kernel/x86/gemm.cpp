#include <iostream>
#include <string.h>
#include <vector>
#include "ecas/ecas.hpp"

namespace ecas {

void Gemm(const int M, const int N, const int K, 
		  const float ALPHA,
		  const float *A, const int lda,
		  const float *B, const int ldb,
		  float *C, const int ldc) {
	printf("gemm v1.\n");
	int i, j, k;
	memset(C, 0, sizeof(float) * ldc * M);
	for (i = 0; i < M; ++i) {
		for (k = 0; k < K; ++k) {
			register float A_PART = ALPHA * A[i * lda + k];
			for (j = 0; j < N; ++j) {
				C[i * ldc + j] += A_PART * B[k * ldb + j];
			}
		}
	}
}

} // ecas.