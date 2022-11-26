#ifndef DLAS_CXX_API_HPP_
#define DLAS_CXX_API_HPP_

#include <string>

namespace dlas {

enum SessionMode {
    SERIAL = 0,   // Single thread.
    PARALLEL      // Multiple threads.
};

// Session
class Session {
public:
    Session(const std::string& name, SessionMode mode);
    ~Session();

    void Run();

private:
    void *params_;
};

void HelloWorld();
void VulkanHelloWorld();

} // dlas.

#endif // DLAS_CXX_API_HPP_