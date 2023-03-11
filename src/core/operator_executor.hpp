/*!
* \brief 
*/

#ifndef ECAS_CORE_OPERATOR_EXECUTOR_HPP_
#define ECAS_CORE_OPERATOR_EXECUTOR_HPP_

#include <string>
#include <map>
#include "node.hpp"
#include "backend/operator.hpp"

namespace ecas {

class OperatorExecutor {
    
public:
    OperatorExecutor() {};
    ~OperatorExecutor() {};

    // 
    void *GetOp(std::string op_name) {}
    void OpRun(void *op_ptr, std::vector<Param> &params, 
               std::vector<ITensor *> &inputs, std::vector<ITensor *> &outputs);

private:
    
    std::map<std::string, Operator*> op_map_;
};

}  // end of namespace ecas.

#endif //ECAS_CORE_OPERATOR_EXECUTOR_HPP_