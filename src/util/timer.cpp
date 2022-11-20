/*!
* \brief timer.
*/

#include "timer.hpp"

namespace dlas {
namespace util {

Timer::Timer(std::string name) :
    name_(name),
    count_(0), ave_(0),
    min_(FLT_MAX), max_(0) {}

void Timer::Start() {
    if (is_record_)
        timer_.Start();
}

void Timer::Stop() {
    if (is_record_) {
        timer_.Stop();
        float time = timer_.MilliSeconds();
        if (time > max_) {
            max_ = time;
        }
        if (time < min_) {
            min_ = time;
        }
        ave_ = (count_ / (count_ + 1.0)) * ave_ + time / (count_ + 1.0);
        count_++;
    }
}

bool Timer::is_record_ = false;

} // util.
} // dlas.
