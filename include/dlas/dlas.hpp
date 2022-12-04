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

    void CreateInput(std::vector<unsigned int> &&shape);
    void CreateInput(std::vector<std::vector<unsigned int>> &&shapes);
    void CreateOutput(std::vector<unsigned int> &&shape);
    void CreateOutput(std::vector<std::vector<unsigned int>> &&shapes);

    void CreateNode(const std::string &name, OpTag op_tag);
    void CreateNode(const std::string &name, Task &&task);
    
    void Run();

private:
    void *params_;
};

void HelloWorld();
void VulkanHelloWorld();

} // dlas.

#endif // DLAS_CXX_API_HPP_