#include "dlas/dlas.hpp"

void TaskA(void *in, void *out) {
    printf("TaskA.\n");
}
void TaskB(void *in, void *out) {
    printf("TaskB.\n");
}
void TaskC(void *in, void *out) {
    printf("TaskC.\n");
}

int main() {
    dlas::HelloWorld();
    dlas::VulkanHelloWorld();
    
    dlas::SessionConfig config;
    config.mode = dlas::ExecutionMode::SINGLE;
    config.num_thread = 1;
    dlas::Session *session = new dlas::Session("s1", config);

    session->CreateNode("n1", TaskA);
    session->CreateNode("n2", TaskB);
    session->CreateNode("n3", TaskC);

    session->CreateNode("n4", {{"n1", "n2"}, {"n2", "n3"}});
    session->BuildGraph({{"n1", "n2"}, {"n2", "n3"}});
    session->Group({{"n1"}, {"n2", "n3"}});
    session->ShowInfo();

    // session->GetNodeIO("n1", Tenosr *in, Tensor *out);

    dlas::ITensor in;
    in.data = (char *)malloc(sizeof(float) * 2);
    float *a = (float *)in.data;
    a[0] = 1; a[1] = 2;
    session->Feed(in);
    dlas::ITensor *out;
    session->GetResult(&out);

    return 0;
}