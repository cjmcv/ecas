#include "dlas/dlas.hpp"

void TaskA(void *in, void *out) {
    printf("TaskA.\n");
}

int main() {
    dlas::HelloWorld();
    dlas::VulkanHelloWorld();
    
    dlas::SessionConfig config;
    config.mode = dlas::ExecutionMode::SINGLE;
    config.num_thread = 1;
    dlas::Session *session = new dlas::Session("s1", config);

    session->CreateInput({{2, 2}, 
                          {2, 2}});
    session->CreateOutput({2, 2});

    session->CreateNode("n1", dlas::OpTag::VECTOR_ADD);
    session->CreateNode("n2", TaskA);

    // TODO: n1 + n2 => g1
    session->Run("n2");
    
    return 0;
}