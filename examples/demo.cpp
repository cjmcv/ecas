#include "dlas/dlas.hpp"

void TaskA(void *in, void *out) {
    printf("TaskA.\n");
}
void TaskB(void *in, void *out) {
    printf("TaskB.\n");
}

int main() {
    dlas::HelloWorld();
    dlas::VulkanHelloWorld();
    
    dlas::SessionConfig config;
    config.mode = dlas::ExecutionMode::SINGLE;
    config.num_thread = 1;
    dlas::Session *session = new dlas::Session("s1", config);

    session->CreateNode("n1", dlas::OpTag::VECTOR_ADD);
    session->CreateNode("n2", TaskA);
    session->CreateNode("n3", TaskB);

    session->CreateNode("n4", {{"n1", "n2"}, {"n2", "n3"}});
    session->BuildGraph({{"n1", "n2"}, {"n2", "n3"}});


    // session->GetNodeIO("n1", Tenosr *in, Tensor *out);


    // TODO: n1 + n2 => g1
    session->Run("n1");
    
    return 0;
}