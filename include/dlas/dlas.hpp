#ifndef DLAS_CXX_API_HPP_
#define DLAS_CXX_API_HPP_

#include <string>
#include <functional>

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

enum OpTag {
    CUSTOM = 0, // 
    VECTOR_ADD, 
};

struct SessionConfig {
    ExecutionMode mode;
    int num_thread;
};

using Task = std::function<void(void *input, void *output)>;

// Session
class DLAS_API Session {
public:
    Session(const std::string &name, SessionConfig &config);
    ~Session();
    void CreateNode(const std::string &name, OpTag op_tag);
    void CreateNode(const std::string &name, Task &&c);
    void Run();

private:
    void *params_;
};

void HelloWorld();
void VulkanHelloWorld();

} // dlas.

#endif // DLAS_CXX_API_HPP_