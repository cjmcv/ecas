#include "kernel_dispatcher.hpp"

#include "x86/kernels_list.hpp"

namespace ecas {

CpuKernelDispatcher::CpuKernelDispatcher() {
	GemmKernel = Gemm;
	DotKernel = Dot;
}

} // ecas.