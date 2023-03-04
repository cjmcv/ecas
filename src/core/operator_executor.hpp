/*!
* \brief 
*/

#ifndef ECAS_CORE_OPERATOR_EXECUTOR_HPP_
#define ECAS_CORE_OPERATOR_EXECUTOR_HPP_

#include <string>
#include "node.hpp"

namespace ecas {

class OperatorExecutor {
    
public:
    ~OperatorExecutor() {};

    // Singleton mode. Only one OperatorExecutor exist.
    static OperatorExecutor &GetInstance() {
        static OperatorExecutor exe;
        return exe;
    }

    // 
    void OpRun(std::string op_name, std::vector<Param> &params, 
               std::vector<ITensor *> &inputs, std::vector<ITensor *> &outputs);

private:
    OperatorExecutor() {};
};

}  // end of namespace ecas.

#endif //ECAS_CORE_OPERATOR_EXECUTOR_HPP_