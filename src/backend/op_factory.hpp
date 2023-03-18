/*!
* \brief 
*/

#ifndef ECAS_BACKEND_OPERATOR_FACTORY_HPP_
#define ECAS_BACKEND_OPERATOR_FACTORY_HPP_

#include <string>
#include <map>

#include "operator.hpp"

namespace ecas {

class OpFactory {
    // A function pointer.
    typedef Operator *(*OpCreator)(std::string &params_str);

public:
    class ObjectRegisterHelper {
    public:
        ObjectRegisterHelper(std::string op_name, OpCreator creator) {
            printf("register: %s.\n", op_name.c_str());
            OpFactory::GetInstance().RegisterOpClass(op_name, creator);
        }
    };

    ~OpFactory(){};

    // Create the op object by the name of the op.
    Operator *CreateOpByName(std::string &op_name, std::string &param_str);

    // Registerer, set the mapping relation between operator's class name and its constructor.
    void RegisterOpClass(std::string &op_name, OpCreator creator);

    // Show which ops have been registered.
    std::string PrintList();

    // Singleton mode. Only one OpFactory exist.
    static OpFactory &GetInstance() {
        static OpFactory factory;
        return factory;
    }

private:
    OpFactory() {};
    // <name, creater>
    std::map<std::string, OpCreator> op_creator_map_;
};

#define OPERATOR_REGISTER_NAME(n) register_op_##n##_
#define OPERATOR_REGISTER(name, creator) __attribute__((unused)) OpFactory::ObjectRegisterHelper OPERATOR_REGISTER_NAME(name)(#name, creator)

}  // end of namespace ecas.

#endif // ECAS_BACKEND_OPERATOR_FACTORY_HPP_ 