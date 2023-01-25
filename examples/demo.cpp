#include "ecas/ecas.hpp"

void TaskA(void *in, void *out) {
    printf("TaskA.\n");
}
void TaskB(void *in, void *out) {
    printf("TaskB.\n");
}
void TaskC(void *in, void *out) {
    printf("TaskC.\n");
}
void TaskD(void *in, void *out) {
    printf("TaskD.\n");
}

int main() {
    ecas::HelloWorld();
    ecas::VulkanHelloWorld();
    
    ecas::SessionConfig config;
    config.mode = ecas::ExecutionMode::SINGLE;
    config.num_thread = 1;
    ecas::Session *session = new ecas::Session("s1", config);

    // TODO: BuildGraph后将完成group的确认和tensor/Blockingqueue的分配。
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

    session->Start();
    session->Feed(in);
    printf("Finish Feed.\n");
    ecas::ITensor *out;
    session->GetResult(&out);
    session->Stop();
    return 0;
}