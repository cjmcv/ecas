/*!
* \brief . 
*/

#include "backend/dot_op.hpp"
#include "core/allocator.hpp"
#include "core/tensor.hpp"

#include "gtest/gtest.h"

namespace {

using namespace ecas;

void DotOpTest() {
    std::string op_params;
    Operator *op = DotOp::Creator(op_params);

    Allocator allocator;

    std::vector<ITensor *> inputs;
    std::vector<ITensor *> outputs;

    int len = 300;
    std::vector<int> shapei = {len};
    inputs.push_back(allocator.CreateTensor(shapei, ecas::FP32, nullptr)); // TODO: 内存未分配
    inputs.push_back(allocator.CreateTensor(shapei, ecas::FP32, nullptr));

    float *in_data0 = (float *)inputs[0]->GetData();
    float *in_data1 = (float *)inputs[1]->GetData();
    for (int i=0; i<len; i++) {
        in_data0[i] = 1;
        in_data1[i] = 2;
    }
    std::vector<int> shapeo = {1};
    outputs.push_back(allocator.CreateTensor(shapeo, ecas::FP32, nullptr));

    std::vector<Param> params;
    op->Run(params, inputs, outputs);
    EXPECT_EQ(600, ((float *)outputs[0]->GetData())[0]);
}

TEST(OpTest, Dot) {
    DotOpTest();
}

}  // end of namespace.