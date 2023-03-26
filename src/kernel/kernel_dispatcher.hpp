#include <iostream>
#include <string.h>
#include "ecas/ecas.hpp"

namespace ecas {

// TODO: 当前目录新增cmakelist，管理各后端目录下的cmakelist。

// Kernel分配，对外每个函数只会提供一种CPU加速方式。
// TODO: 针对gpu kernel，可能需要额外的参数，这些通过补充成员来实现。
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

// TODO: 1. 根据型号，确定线程布局。2. 选择spirv文件。
//       原则：尽可能不添加太多kernel外的内容，使这部分代码可以快速解耦使用到其他地方，也方便后面快速加入其他地方的代码。
class VulkanKernelDispatcher {
public:
    // MatMul

	void Init() {}

	// Singleton mode. Only one KernelFactory exist.
	static VulkanKernelDispatcher& GetInstance() {
		static VulkanKernelDispatcher dispatcher;
		return dispatcher;
	}

private:
	VulkanKernelDispatcher() {}
};

} // ecas.