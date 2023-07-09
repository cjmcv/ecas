#include "kernel_dispatcher.hpp"

namespace ecas {

// kernel lists.
void Dot(int len, const float *vec_a, const float *vec_b, float *res);

void Gemm(const int M, const int N, const int K, 
		  const float ALPHA,
		  const float *A, const int lda,
		  const float *B, const int ldb,
		  float *C, const int ldc);

CpuKernelDispatcher::CpuKernelDispatcher() {
	GemmKernel = Gemm;
	DotKernel = Dot;
}

// VulkanKernelDispatcher::VulkanKernelDispatcher() {

// }

} // ecas.