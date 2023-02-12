#include "ecas/ecas.hpp"

#include <thread>

void TaskA(std::vector<ecas::ITensor *> &input, std::vector<ecas::ITensor *> &output) {
    static int count = 0;
    printf("TaskA: %d.\n", count++);
}
void TaskB(std::vector<ecas::ITensor *> &input, std::vector<ecas::ITensor *> &output) {
    static int count = 0;
    printf("TaskB: %d.\n", count++);
}
void TaskC(std::vector<ecas::ITensor *> &input, std::vector<ecas::ITensor *> &output) {
    static int count = 0;
    printf("TaskC: %d.\n", count++);
}
void TaskD(std::vector<ecas::ITensor *> &input, std::vector<ecas::ITensor *> &output) {
    static int count = 0;
    printf("TaskD: %d.\n", count++);
}

int main() {
    ecas::HelloWorld();
    // ecas::VulkanHelloWorld();
    
    ecas::SessionConfig config;
    config.mode = ecas::ExecutionMode::SINGLE;
    config.num_thread = 1;
    ecas::Session *session = new ecas::Session("s1", config);

    // TODO: 简化API
    session->CreateNode("n1", TaskA, {{2}}, {{2, 2}, {3, 3}}, 0);
    session->CreateNode("n2", TaskB, {{2, 2}}, {{2, 2}}, 1);
    session->CreateNode("n3", TaskC, {{3, 3}}, {{3, 3}}, 0);
    session->CreateNode("n4", TaskD, {{2, 2}, {3, 3}}, {{2}}, 0);
    session->CreateNode("n5", {{"n1", "n2"}, {"n2", "n3"}});
    
    session->BuildGraph({{"n1", "n2"}, {"n1", "n3"}, {"n2", "n4"}, {"n3", "n4"}});
    // session->Group({{"n1"}, {"n2", "n3"}});
    // session->Group({{"n1", "n2", "n3"}});
    session->ShowInfo();

    // session->GetNodeIO("n1", Tenosr *in, Tensor *out);

    ecas::ITensor in;
    in.data = (char *)malloc(sizeof(float) * 2);
    float *a = (float *)in.data;
    a[0] = 1; a[1] = 2;
    in.shape = {2};
    in.mem_type = ecas::MEMORY_HOST;

    ecas::ITensor out;
    out.data = (char *)malloc(sizeof(float) * 2);
    out.shape = {2};
    out.mem_type = ecas::MEMORY_HOST;

    session->Start();
    for (int i=0; i<5; i++) {
        session->Feed(in);
    }
    for (int i=0; i<5; i++) {
        session->GetResult(&out);
    }
    std::this_thread::sleep_for(std::chrono::seconds(2));
    printf("Call stop.\n");
    session->Stop();
    return 0;
}