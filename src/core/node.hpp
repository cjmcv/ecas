/*!
* \brief Compute node.
*/

#ifndef ECAS_CORE_NODE_HPP_
#define ECAS_CORE_NODE_HPP_

#include <string>
#include "ecas/ecas.hpp"
#include "tensor_pool.hpp"

namespace ecas {

// TODO: 内存管理，内存复用在其内部接allocator
// class Tensor;
// TODO: 入参，算法参数配置，按类型分组，
// struct Params;
// TODO: 可选择已注册的kernel函数，也可以外设自己的函数

class Node {
public:
    Node(): input_nodes_(nullptr), output_nodes_(nullptr) {}
    virtual ~Node() {};
    virtual void Run(ITensor *input, ITensor *output) = 0;

    inline std::string &name() { return name_; }
    inline void SetInputNodes(std::vector<Node *> *input_nodes) { input_nodes_ = input_nodes; };
    inline void SetOutputNodes(std::vector<Node *> *output_nodes) { output_nodes_ = output_nodes; };

    inline std::vector<Node *> *input_nodes() { return input_nodes_; }
    inline std::vector<Node *> *output_nodes() { return output_nodes_; }

    inline std::vector<std::vector<int>> &input_shapes() { return input_shapes_; }
    inline std::vector<std::vector<int>> &output_shapes() { return output_shapes_; }

    inline void AppendInputs(BlockingQueuePair *bq) { inputs_.push_back(bq); }
    inline void AppendOutputs(BlockingQueuePair *bq) { outputs_.push_back(bq); }
    inline std::vector<BlockingQueuePair *> &inputs() { return inputs_; }
    inline std::vector<BlockingQueuePair *> &outputs() { return outputs_; }

    bool CheckInputIsReady();
    void GetInputsBuffer();

protected:
    std::string name_;
    OpTag op_tag_;

    std::vector<Node *> *input_nodes_;
    std::vector<Node *> *output_nodes_;

    std::vector<std::vector<int>> input_shapes_;
    std::vector<std::vector<int>> output_shapes_;

    std::vector<BlockingQueuePair *> inputs_; // It is also part of the output of the input node 
    std::vector<BlockingQueuePair *> outputs_; // It is also part of the input of the output node
};

}  // end of namespace ecas.

#endif //ECAS_CORE_NODE_HPP_