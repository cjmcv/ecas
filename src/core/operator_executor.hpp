/*!
* \brief 
*/

#ifndef ECAS_CORE_OPERATOR_EXECUTOR_HPP_
#define ECAS_CORE_OPERATOR_EXECUTOR_HPP_

#include <string>
#include <map>
#include "backend/operator.hpp"
#include "backend/op_factory.hpp"

namespace ecas {

class OperatorExecutor {
    
public:
    OperatorExecutor();
    ~OperatorExecutor();

    // 
    Operator *CreateOp(std::string &op_name, std::string &op_params);
    void OpRun(void *op_ptr, std::vector<Param> &params, 
               std::vector<ITensor *> &inputs, std::vector<ITensor *> &outputs);

private:
    std::vector<Operator*> ops_;
    // std::map<std::string, Operator*> op_map_;
};

}  // end of namespace ecas.

#endif //ECAS_CORE_OPERATOR_EXECUTOR_HPP_