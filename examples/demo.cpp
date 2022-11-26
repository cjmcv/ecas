#include "dlas/dlas.hpp"

int main() {
    dlas::HelloWorld();
    dlas::VulkanHelloWorld();
    
    dlas::Session *session = new dlas::Session("s1", dlas::SessionMode::SERIAL);
    session->Run();
    return 0;
}