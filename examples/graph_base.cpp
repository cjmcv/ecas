#include "ecas/ecas.hpp"

#include <chrono>
#include <thread>
#include <cmath>

class AlgoTasks {
public:
    AlgoTasks(ecas::Session *session) {
        session_ = session;

        std::string op_params = "alpha: 1.0, beta: 2.0";
        gemm_ptr_ = session_->CreateOp("gemm", op_params);
        dot_ptr_ = session_->CreateOp("dot", op_params);

        // for task B and C
        gemm_b_ = session_->CreateITensor({600, 300}, ecas::FP32);
        float *data = (float *)gemm_b_->GetData();
        for (int j=0; j<600*300; j++) {
            data[j] = 1;
        }
        // for task D
        dot_a_ = session_->CreateITensor({300}, ecas::FP32);
        dot_b_ = session_->CreateITensor({300}, ecas::FP32);
    }
    ecas::Session *session() { return session_; };

public:
    ecas::ITensor *gemm_b_;
    void *gemm_ptr_;

    ecas::ITensor *dot_a_;
    ecas::ITensor *dot_b_;
    void *dot_ptr_;

private:
    ecas::Session *session_;
};

// 转置 分割 -> 乘法  ->  累加 转置？
//          -> 乘法 
// 600 * 600 -> 转置 -> 分割 200 * 600 ， 400 * 600
void TaskA(void *usr, std::vector<ecas::ITensor *> &inputs, std::vector<ecas::ITensor *> &outputs) {
    AlgoTasks *ins = (AlgoTasks *)usr;
    static int count = 0;
    // TODO: 检查维度（用static 检查一次即可），宏定义，归到工具中
    float *data = (float *)inputs[0]->GetData();
    int rows = inputs[0]->shape()[0];
    int cols = inputs[0]->shape()[1];

    for (int i=0; i<rows; i++) {
        for (int j=i+1; j<cols; j++) {
            float temp = data[i*cols + j];
            data[i*cols + j] = data[j*cols + i];
            data[j*cols + i] = temp;
        }
    }

    float *out_data0 = (float *)outputs[0]->GetData();
    float *out_data1 = (float *)outputs[1]->GetData();
    for (int i=0; i<rows/2; i++) {
        for (int j=0; j<cols; j++) {
            out_data0[i*cols + j] = data[i*cols + j];
        }
    }
    for (int i=rows/2; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            out_data1[(i-rows/2)*cols + j] = data[i*cols + j] + 1;
        }
    }
    printf("TaskA: %d (%d).\n", count++, std::this_thread::get_id());
}

// 200 * 600 -> gemm（600 * 300）-> 200 * 300 
void TaskB(void *usr, std::vector<ecas::ITensor *> &inputs, std::vector<ecas::ITensor *> &outputs) {
    AlgoTasks *ins = (AlgoTasks *)usr;

    static int count = 0;

    std::vector<ecas::ITensor *> new_inputs;
    new_inputs.push_back(inputs[0]);
    new_inputs.push_back(ins->gemm_b_);
    std::vector<ecas::Param> params;
    ins->session()->OpRun(ins->gemm_ptr_, params, new_inputs, outputs);

    // outputs[0]->Print();
    printf("TaskB: %d (%d).\n", count++, std::this_thread::get_id());
}

// 400 * 600 -> gemm（600 * 300）-> 400 * 300
void TaskC(void *usr, std::vector<ecas::ITensor *> &inputs, std::vector<ecas::ITensor *> &outputs) {
    AlgoTasks *ins = (AlgoTasks *)usr;

    static int count = 0;

    std::vector<ecas::ITensor *> new_inputs;
    new_inputs.push_back(inputs[0]);
    new_inputs.push_back(ins->gemm_b_);
    std::vector<ecas::Param> params;
    ins->session()->OpRun(ins->gemm_ptr_, params, new_inputs, outputs);

    // outputs[0]->Print();
    printf("TaskC: %d (%d).\n", count++, std::this_thread::get_id());
}

// 200 * 300， 400 * 300 合并 点积分
void TaskD(void *usr, std::vector<ecas::ITensor *> &inputs, std::vector<ecas::ITensor *> &outputs) {
    AlgoTasks *ins = (AlgoTasks *)usr;
    static int count = 0;

    float *data0 = (float *)inputs[0]->GetData();
    float *data1 = (float *)inputs[1]->GetData();
    ins->dot_a_->BindHostDataPtr(data0);
    ins->dot_b_->BindHostDataPtr(data1);

    std::vector<ecas::ITensor *> new_inputs;
    new_inputs.push_back(ins->dot_a_);
    new_inputs.push_back(ins->dot_b_);
    std::vector<ecas::Param> params;
    ins->session()->OpRun(ins->dot_ptr_, params, new_inputs, outputs);

    // printf("inner id: %d.\n", );
    outputs[0]->Print();
    printf("TaskD: %d (%d).\n", count++, std::this_thread::get_id());
}

class SerialPass {
public:
    void Initialize(AlgoTasks *ins) {
        ins_ = ins;
        a_in_ = ins->session()->CreateITensor({600, 600}, ecas::FP32);
        a_out_b_in_ = ins->session()->CreateITensor({200, 600}, ecas::FP32);
        a_out_c_in_ = ins->session()->CreateITensor({400, 600}, ecas::FP32);

        b_out_d_in_ = ins->session()->CreateITensor({200, 300}, ecas::FP32);
        c_out_d_in_ = ins->session()->CreateITensor({400, 300}, ecas::FP32);

        d_out_ = ins->session()->CreateITensor({1}, ecas::FP32);
        //
        // a_vin_.push_back(a_in_);
        a_vout_.push_back(a_out_b_in_);
        a_vout_.push_back(a_out_c_in_);

        b_vin_.push_back(a_out_b_in_);
        b_vout_.push_back(b_out_d_in_);

        c_vin_.push_back(a_out_c_in_);
        c_vout_.push_back(c_out_d_in_);

        d_vin_.push_back(b_out_d_in_);
        d_vin_.push_back(c_out_d_in_);
        // d_vout_.push_back(d_out_);
    }

    void Run(std::vector<ecas::ITensor *> &inputs, std::vector<ecas::ITensor *> &outputs) {
        TaskA(ins_, inputs, a_vout_);
        TaskB(ins_, b_vin_, b_vout_);
        TaskC(ins_, c_vin_, c_vout_);
        TaskD(ins_, d_vin_, outputs);
    }
    
private:
    AlgoTasks *ins_;

    ecas::ITensor *a_in_;
    ecas::ITensor *a_out_b_in_;
    ecas::ITensor *a_out_c_in_;

    ecas::ITensor *b_out_d_in_;
    ecas::ITensor *c_out_d_in_;

    ecas::ITensor *d_out_;

    std::vector<ecas::ITensor *> a_vin_;
    std::vector<ecas::ITensor *> a_vout_;

    std::vector<ecas::ITensor *> b_vin_;
    std::vector<ecas::ITensor *> b_vout_;

    std::vector<ecas::ITensor *> c_vin_;
    std::vector<ecas::ITensor *> c_vout_;

    std::vector<ecas::ITensor *> d_vin_;
    std::vector<ecas::ITensor *> d_vout_;
};

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

    //
    ecas::UtilBox util_box;
    void *timer = util_box.GetNewTimer("graph_base", 2);

    util_box.TimerStart(timer);
    AlgoTasks algo(session);
    session->Start((void *)&algo);
    float *in_data = (float *)in->GetData();
    for (int i=0; i<5; i++) {
        for (int j=0; j<600*600; j++) {
            in_data[j] = 1;
        }
        in->SetId(i);
        session->GraphFeed(in);
    }
    for (int i=0; i<5; i++) {
        session->GraphGetResult(out);
        printf("out id: %d, %f.\n", out->id(), ((float *)out->GetData())[0]);
    }
    util_box.TimerStop(timer, 0);

    // std::this_thread::sleep_for(std::chrono::seconds(2));
    printf("Call stop.\n");
    session->Stop();

    // SerialPass demo.
    SerialPass sp;
    sp.Initialize(&algo);
    std::vector<ecas::ITensor *> a_vin;
    a_vin.push_back(in);
    std::vector<ecas::ITensor *> d_vout;
    d_vout.push_back(out);

    util_box.TimerStart(timer);
    for (int i=0; i<5; i++) {
        for (int j=0; j<600*600; j++) {
            in_data[j] = 1;
        }
        in->SetId(i+5);
        sp.Run(a_vin, d_vout);
        printf("out id: %d, %f.\n", out->id(), ((float *)out->GetData())[0]);
    }
    util_box.TimerStop(timer, 1, 1);

    //

    float y = ecas::Math::expf(1.234f);
    float y2 = expf(1.234f);
    printf("expf(1.234f): %f, %f.\n", y, y2);
}