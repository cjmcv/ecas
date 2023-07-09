/*!
* \brief Operator. 
*/

#ifndef ECAS_BACKEND_OPERATOR_HPP_
#define ECAS_BACKEND_OPERATOR_HPP_

#include "ecas/ecas.hpp"
#include "kernel/x86/kernel_dispatcher.hpp"

namespace ecas {

// TODO: 操作子基类，派生出各种加速用的操作子（对kernel参数进行包装），派生操作子对kernel不是一一对应，以免操作子过多。
//       派生操作子分为几大类，再往下分函数类。跟kernel函数组一一对应？包含参数包装，参数描述，资源管理等。
//       不需要再用kernel工厂，kernel按类别直接被对应派生操作子引用。
//       kernel分inplace / 非inplace两种单函数，以及组合函数。可使用公用辅助内存。
//       kernel保持干净解耦
//       添加kernel额外辅助参数管理，如内存池

//       派生cpu，vulkan，gpu子类？

// 负责对接Kernel Dispatcher进行封装，对Session输出
class Operator {

public:
    Operator(): cpu_dispatcher_(CpuKernelDispatcher::GetInstance()) {}
    virtual bool DimCheck(std::vector<Param> &params, std::vector<ITensor *> &inputs, std::vector<ITensor *> &outputs) = 0;
    virtual void Run(std::vector<Param> &params, std::vector<ITensor *> &inputs, std::vector<ITensor *> &outputs) = 0;

    // Show relevant prompts.
    virtual void Help() const = 0;

// private:
//     // Set the input and output data.
//     virtual void IoCheckAndSet(const std::vector<ITensor *> &inputs,
//                                const std::vector<ITensor *> &outputs) = 0;

protected:
    CpuKernelDispatcher *cpu_dispatcher_;
    
};

}  // end of namespace ecas.

#endif // ECAS_BACKEND_OPERATOR_HPP_