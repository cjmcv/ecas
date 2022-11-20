/*!
* \brief Internal Thread.
*        Mainly implemented by thread.
*/

#ifndef DLAS_UTIL_INTERNAL_THREAD_HPP_
#define DLAS_UTIL_INTERNAL_THREAD_HPP_

#include <thread>

namespace dlas {
namespace util {

class InternalThread {
public:
    InternalThread() : thread_(), interrupt_flag_(false) {}
    virtual ~InternalThread() { Stop(); }

    // To chech wether the internal thread has been started.
    inline bool is_started() const { return thread_ && thread_->joinable(); }

    bool Start();
    void Stop();

protected:
    // Virtual function, should be override by the classes
    // which needs a internal thread to assist.
    virtual void Entry() {}
    bool IsMustStop();

private:
    bool interrupt_flag_;
    std::shared_ptr<std::thread> thread_;
};

} // namespace util
} // namespace dlas

#endif // DLAS_UTIL_INTERNAL_THREAD_HPP_
