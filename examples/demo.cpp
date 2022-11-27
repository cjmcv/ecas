#include "dlas/dlas.hpp"

int main() {
    dlas::HelloWorld();
    dlas::VulkanHelloWorld();
    
    dlas::SessionConfig config;
    config.mode = dlas::ExecutionMode::DLAS_SINGLE;
    config.num_thread = 1;
    dlas::Session *session = new dlas::Session("s1", config);
    session->Run();
    return 0;
}