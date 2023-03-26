/*!
* \brief Operator. 
*/

#ifndef ECAS_BACKEND_OPERATOR_DOT_HPP_
#define ECAS_BACKEND_OPERATOR_DOT_HPP_

#include "operator.hpp"

namespace ecas {

struct DotKernelParam {};

class DotOp: public Operator {
public:
    static Operator *Creator(std::string &params_str);
    DotOp(DotKernelParam &params) :Operator() {
        params_ = params;
    }

    void Help() const;
    bool DimCheck(std::vector<Param> &params, std::vector<ITensor *> &inputs, std::vector<ITensor *> &outputs);
    void Run(std::vector<Param> &params, std::vector<ITensor *> &inputs, std::vector<ITensor *> &outputs);

private:
    DotKernelParam params_;
};


}  // end of namespace ecas.

#endif // ECAS_BACKEND_OPERATOR_DOT_HPP_