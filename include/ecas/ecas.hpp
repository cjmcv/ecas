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

enum MemoryType {
    MEMORY_HOST = 0,
    MEMORY_DEVICE, 
};

struct SessionConfig {
    ExecutionMode mode;
    int num_thread;
};

struct ITensor {
    MemoryType mem_type;
    int id;
    char *data;
    std::vector<int> shape;
};

using Task = std::function<void(std::vector<ITensor *> &input, std::vector<ITensor *> &output)>;

// Session
class ECAS_API Session {
public:
    Session(const std::string &name, SessionConfig &config);
    ~Session();

    void CreateNode(const std::string &name, Task &&task, 
                    std::vector<std::vector<int>> &&in_shapes, 
                    std::vector<std::vector<int>> &&out_shapes,
                    int group_id = 0);
    void CreateNode(const std::string &name, std::vector<std::vector<std::string>> &&relation);
    void BuildGraph(std::vector<std::vector<std::string>> &&relation);
    void ShowInfo();

    void Start();
    void Stop();

    // Asynchronous function.
    void Feed(ITensor &in);
    // Get the result after calling the Feed.
    void GetResult(ITensor *out);

    ITensor *CreateTensor(std::vector<int> &&shape);
    
private:
    void *params_;
};

// Independent acceleration functions
void HelloWorld();
void VulkanHelloWorld();

} // ecas.

#endif // ECAS_CXX_API_HPP_