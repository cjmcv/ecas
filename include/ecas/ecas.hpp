#ifndef ECAS_CXX_API_HPP_
#define ECAS_CXX_API_HPP_

#include <string>
#include <vector>
#include <functional>

namespace ecas {

#if defined(_MSC_VER)
#define ECAS_API __declspec(dllexport)
#else
#define ECAS_API __attribute__((visibility("default")))
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

struct ITensor {
    char *data;
    int len;
};

using Task = std::function<void(ITensor *input, ITensor *output)>;

// Session
class ECAS_API Session {
public:
    Session(const std::string &name, SessionConfig &config);
    ~Session();

    void CreateNode(const std::string &name, OpTag op_tag);
    void CreateNode(const std::string &name, Task &&task);
    void CreateNode(const std::string &name, std::vector<std::vector<std::string>> &&relation);

    void BuildGraph(std::vector<std::vector<std::string>> &&relation);
    void Group(std::vector<std::vector<std::string>> &&groups);

    void ShowInfo();
    void Feed(ITensor &in);
    void GetResult(ITensor **out);

private:
    void *params_;
};

// Independent acceleration functions
void HelloWorld();
void VulkanHelloWorld();

} // ecas.

#endif // ECAS_CXX_API_HPP_