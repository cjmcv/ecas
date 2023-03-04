#include "kernel_dispatcher.hpp"

#include "x86/kernels_list.hpp"

namespace ecas {

void CpuKernelDispatcher::Init() {
	GemmKernel = GemmV1;
}

} // ecas.