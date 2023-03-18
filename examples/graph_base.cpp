#include "ecas/ecas.hpp"

#include <thread>

class AlgoTasks {
public:
    AlgoTasks(ecas::Session *session) {
        session_ = session;

        std::string op_params = "alpha: 1.0, beta: 2.0";
        gemm_ptr_ = session_->CreateOp("gemm", op_params);
    }
    ecas::Session *session() { return session_; };

private:
    ecas::Session *session_;
    void *gemm_ptr_;

};

// 转置 分割 -> 乘法  ->  累加 转置？
//          -> 乘法 
// 600 * 600 -> 转置 -> 分割 200 * 600 ， 400 * 600
void TaskA(void *usr, std::vector<ecas::ITensor *> &inputs, std::vector<ecas::ITensor *> &outputs) {
    AlgoTasks *ins = (AlgoTasks *)usr;
    static int count = 0;
    // TODO: 检查维度（用static 检查一次即可），宏定义，归到工具中
    float *data = (float *)inputs[0]->data;
    int rows = inputs[0]->shape[0];
    int cols = inputs[0]->shape[1];

    for (int i=0; i<rows; i++) {
        for (int j=i+1; j<cols; j++) {
            float temp = data[i*cols + j];
            data[i*cols + j] = data[j*cols + i];
            data[j*cols + i] = temp;
        }
    }

    float *out_data0 = (float *)outputs[0]->data;
    float *out_data1 = (float *)outputs[1]->data;
    for (int i=0; i<rows/2; i++) {
        for (int j=0; j<cols; j++) {
            out_data0[i*cols + j] = data[i*cols + j];
        }
    }
    for (int i=rows/2; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            out_data1[(i-rows/2)*cols + j] = data[i*cols + j];
        }
    }
    printf("TaskA: %d (%d).\n", count++, std::this_thread::get_id());
}
// 200 * 600 -> gemm（600 * 300）-> 200 * 300 
void TaskB(void *usr, std::vector<ecas::ITensor *> &inputs, std::vector<ecas::ITensor *> &outputs) {
    AlgoTasks *ins = (AlgoTasks *)usr;

    static int count = 0;

    // TODO: 把算法类作为参数传入到Task中。
    // TODO: gemm为两输入，需要使用CreateITensor多补一个。
    // ecas::ITensor *in = session->CreateITensor({600, 600}, ecas::FP32);
    // ecas::Param param;
    // param.fval = 1.0;
    // std::vector<ecas::Param> gemm_params;
    // gemm_params.push_back(param);
    // ecas::Session::OpRun("gemm", gemm_params, inputs, outputs);

    // float *data = (float *)inputs[0]->data;
    // int rows = inputs[0]->shape[0];
    // int cols = inputs[0]->shape[1];

    // for (int i=0; i<rows; i++) {
    //     for (int j=0; j<cols; j++) {
    //         printf("%f, ", data[i*cols + j]);
    //     }
    //     printf("\n");
    // }
    printf("TaskB: %d (%d).\n", count++, std::this_thread::get_id());
}
// 400 * 600 -> gemm（600 * 300）-> 400 * 300
void TaskC(void *usr, std::vector<ecas::ITensor *> &inputs, std::vector<ecas::ITensor *> &outputs) {
    static int count = 0;

    // float *data = (float *)inputs[0]->data;
    // int rows = inputs[0]->shape[0];
    // int cols = inputs[0]->shape[1];

    // for (int i=0; i<rows; i++) {
    //     for (int j=0; j<cols; j++) {
    //         printf("%f, ", data[i*cols + j]);
    //     }
    //     printf("\n");
    // }
    printf("TaskC: %d (%d).\n", count++, std::this_thread::get_id());
}
// 200 * 300， 400 * 300 合并 点积分
void TaskD(void *usr, std::vector<ecas::ITensor *> &inputs, std::vector<ecas::ITensor *> &outputs) {
    static int count = 0;
    printf("TaskD: %d (%d).\n", count++, std::this_thread::get_id());
}

void GraphBaseDemo() {

    ecas::SessionConfig config;
    config.mode = ecas::ExecutionMode::SINGLE;
    config.num_thread = 1;
    ecas::Session *session = new ecas::Session("s1", config);

    session->CreateNode("n1", TaskA, {{ecas::FP32, 600, 600}}, {{ecas::FP32, 200, 600}, {ecas::FP32, 400, 600}}, 0);
    session->CreateNode("n2", TaskB, {{ecas::FP32, 200, 600}}, {{ecas::FP32, 200, 300}}, 1);
    session->CreateNode("n3", TaskC, {{ecas::FP32, 400, 600}}, {{ecas::FP32, 400, 300}}, 0);
    session->CreateNode("n4", TaskD, {{ecas::FP32, 200, 300}, {ecas::FP32, 400, 300}}, {{ecas::FP32, 1}}, 0);
    session->CreateNode("n5", {{"n1", "n2"}, {"n2", "n3"}});
    
    session->BuildGraph({{"n1", "n2"}, {"n1", "n3"}, {"n2", "n4"}, {"n3", "n4"}});
    session->ShowInfo();

    ecas::ITensor *in = session->CreateITensor({600, 600}, ecas::FP32);
    ecas::ITensor *out = session->CreateITensor({1}, ecas::FP32);

    AlgoTasks algo(session);
    // TODO: 构建串行测试代码
    session->Start((void *)&algo);
    float *in_data = (float *)in->data;
    for (int i=0; i<5; i++) {
        for (int j=0; j<600*600; j++) {
            in_data[j] = j;
        }
        in->id = i;
        session->GraphFeed(in);
    }
    for (int i=0; i<5; i++) {
        session->GraphGetResult(out);
        printf("out id: %d.\n", out->id);
    }
    std::this_thread::sleep_for(std::chrono::seconds(2));
    printf("Call stop.\n");
    session->Stop();
}