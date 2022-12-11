#ifndef DLAS_CXX_API_HPP_
#define DLAS_CXX_API_HPP_

#include <string>
#include <vector>
#include <functional>

namespace dlas {

#if defined(_MSC_VER)
#define DLAS_API __declspec(dllexport)
#else
#define DLAS_API __attribute__((visibility("default")))
#endif

enum ExecutionMode {
    SINGLE = 0,   // Independent single task
    SERIAL = 1,   // Serial multitasking
    GRAPH         // Multitasking with Computational Graphs
};

enum NodeProperty {
    COMPUTE = 0, 
    INPUT, // 
    OUTPUT,
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
    void CreateNode(const std::string &name, Task &&task);
    void CreateNode(const std::string &name, std::vector<std::vector<std::string>> &&relation);

    void BuildGraph(std::vector<std::vector<std::string>> &&relation);

    void Run(const std::string &name);

private:
    void *params_;
};

void HelloWorld();
void VulkanHelloWorld();

} // dlas.

#endif // DLAS_CXX_API_HPP_