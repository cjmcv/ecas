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

enum DataType {
    FP32 = 0,
    FP16 = 1,
    INT32 = 2,
    INT16 = 3,
    INT8 = 4
};

enum MemoryMode {
    ON_HOST = 0,
    ON_DEVICE, 
};

struct SessionConfig {
    ExecutionMode mode;
    int num_thread;
};

struct ITensor {    
    int id;
    std::vector<int> shape;

    MemoryMode mode;
    DataType type;
    void *data;
};

using Task = std::function<void(std::vector<ITensor *> &inputs, std::vector<ITensor *> &outputs)>;

// Session
class ECAS_API Session {
public:
    Session(const std::string &name, SessionConfig &config);
    ~Session();

    void CreateNode(const std::string &name, Task &&task, 
                    std::vector<std::vector<int>> &&input_dims, 
                    std::vector<std::vector<int>> &&output_dims, 
                    int group_id = 0);
    void CreateNode(const std::string &name, std::vector<std::vector<std::string>> &&relation);
    void BuildGraph(std::vector<std::vector<std::string>> &&relation);
    void ShowInfo(); // 不只是graph的，还包含其他内容

    void Start();
    void Stop();

    // Asynchronous function.
    void GraphFeed(ITensor *in);
    // Get the result after calling the Feed.
    void GraphGetResult(ITensor *out);

    ITensor *CreateITensor(std::vector<int> &&shape, DataType type);
    
private:
    void *params_;
};

// Independent acceleration functions
void HelloWorld();
void VulkanHelloWorld();

} // ecas.

#endif // ECAS_CXX_API_HPP_