#ifndef DLAS_CXX_API_HPP_
#define DLAS_CXX_API_HPP_

#include <string>

namespace dlas {

#if defined(_MSC_VER)
#define DLAS_API __declspec(dllexport)
#else
#define DLAS_API __attribute__((visibility("default")))
#endif

enum ExecutionMode {
    DLAS_SINGLE = 0,   // Independent single task
    DLAS_SERIAL = 1,   // Serial multitasking
    DLAS_GRAPH         // Multitasking with Computational Graphs
};

struct SessionConfig {
    ExecutionMode mode;
    int num_thread;
};

// Session
class DLAS_API Session {
public:
    Session(const std::string &name, SessionConfig &config);
    ~Session();

    void Run();

private:
    void *params_;
};

void HelloWorld();
void VulkanHelloWorld();

} // dlas.

#endif // DLAS_CXX_API_HPP_