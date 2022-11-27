/*!
* \brief Internal Thread.
*        Mainly implemented by thread.
*/

#include "internal_thread.hpp"
#include "logger.hpp"

namespace dlas {
namespace util {

bool InternalThread::IsMustStop() {
    if (thread_ && interrupt_flag_) {
      interrupt_flag_ = false;
      return true;
    }
    return false;
}

bool InternalThread::Start() {
    if (is_started()) {
        DLAS_LOGE("Threads should persist and not be restarted.");
        return false;
    }
    try {
        thread_.reset(new std::thread(&InternalThread::Entry, this));
    }
    catch (std::exception& e) {
        DLAS_LOGE("Thread exception: %s", e.what());
    }
    return true;
}

void InternalThread::Stop() {
    if (is_started()) {
        // This flag will work in must_stop.
        interrupt_flag_ = true;
        try {
            thread_->join();
        }
        catch (std::exception& e) {
            DLAS_LOGE("Thread exception: %s", e.what());
        }
    }
}

} // namespace util
} // namespace dlas

