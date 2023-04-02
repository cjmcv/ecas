/*!
* \brief timer.
*/

#include "timer.hpp"

#include <string.h>
#include "logger.hpp"

namespace ecas {
namespace util {

Timer::Timer(std::string name, uint32_t num) 
            : name_(name), num_(num) {
    count_ = new uint32_t[num]();
    min_ = new float[num]();
    max_ = new float[num]();
    acc_ = new float[num]();

    for (uint32_t i=0; i<num; i++) min_[i] = 999999;
}

Timer::~Timer() {
    delete[] count_;
    delete[] min_;
    delete[] max_;
    delete[] acc_;
}

void Timer::Start() {
    timer_.Start();
}

void Timer::Stop(uint32_t idx, uint32_t print_interval) {
    timer_.Stop();
    float time = timer_.MilliSeconds();

    if (idx >= num_) {
        ECAS_LOGE("Timer::Stop -> idx(%d) >= num_(%d).\n", idx, num_);
    }

    if (time > max_[idx])
        max_[idx] = time;
    if (time < min_[idx])
        min_[idx] = time;

    acc_[idx] += time;
    count_[idx]++;

    if (print_interval != 0 && count_[idx] >= print_interval) {
        for (uint32_t i=0; i<num_; i++) {
            ECAS_LOGS("Timer(%s) idx(%d) cnt(%d): %.3f ms (min: %.3f, max: %.3f).\n", 
                name_.c_str(), i, count_[i], acc_[i] / count_[i], min_[i], max_[i]);
        }
        memset(count_, 0, sizeof(uint32_t) * num_);
        memset(max_, 0, sizeof(float) * num_);
        memset(acc_, 0, sizeof(float) * num_);
        for (uint32_t i=0; i<num_; i++) min_[i] = 999999;
    }
}

} // util.
} // ecas.
