#include <iostream>
#include <string.h>
#include "ecas/ecas.hpp"

namespace ecas {

// Kernel分配，对外每个函数只会提供一种CPU加速方式。
class CpuKernelDispatcher {
public:
	//////////////
	// Level 1
	void (*DotKernel)(int len, const float *vec_a, const float *vec_b, float *res);
	//////////////
	// Level 2

	//////////////
	// Level 3
    // MatMul
    void (*GemmKernel)(const int M, const int N, const int K, const float ALPHA,
		              const float *A, const int lda,
		  			  const float *B, const int ldb,
		              float *C, const int ldc);

	// Singleton mode. Only one KernelFactory exist.
	static CpuKernelDispatcher *GetInstance() {
		static CpuKernelDispatcher *dispatcher = new CpuKernelDispatcher;
		return dispatcher;
	}

private:
	CpuKernelDispatcher();
};

} // ecas.