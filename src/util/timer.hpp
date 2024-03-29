/*!
* \brief timer.
*/

#ifndef ECAS_UTIL_TIMER_HPP_
#define ECAS_UTIL_TIMER_HPP_

#include <iostream>
#include <chrono>

namespace ecas {
namespace util {

// Timer for cpu.
class CpuTimer {
public:
    typedef std::chrono::high_resolution_clock clock;
    typedef std::chrono::nanoseconds ns;

    inline void Start() { start_time_ = clock::now(); }
    inline void Stop() { stop_time_ = clock::now(); }
    inline float NanoSeconds() {
        return (float)std::chrono::duration_cast<ns>(stop_time_ - start_time_).count();
    }

    // Returns the elapsed time in milliseconds.
    inline float MilliSeconds() { return NanoSeconds() / 1000000.f; }

    // Returns the elapsed time in microseconds.
    inline float MicroSeconds() { return NanoSeconds() / 1000.f; }

    // Returns the elapsed time in seconds.
    inline float Seconds() { return NanoSeconds() / 1000000000.f; }

protected:
    std::chrono::time_point<clock> start_time_;
    std::chrono::time_point<clock> stop_time_;
};

class Timer {
public:
    Timer(std::string name, uint32_t num);
    ~Timer();

    void Start();
    void Stop(uint32_t idx, uint32_t print_interval = 0);

private:
    CpuTimer timer_;

    std::string name_;
    uint32_t num_;
    uint32_t *count_;

    float *min_;
    float *max_;
    float *acc_;
};

/////////////////////////////////////////////////
//  auto func = [&]()
//  -> float {
//    timer.Start();
//    ecas::QueryDevices();
//    timer.Stop();
//    return timer.MilliSeconds();
//  };
//  ret = func();
#define TIME_DIFF_RECORD(timer, ...)  \
    [&]() -> void {                     \
        timer.Start();                    \
        {__VA_ARGS__}                     \
        timer.Stop();                     \
    }();

} // util.
} // ecas.
#endif //ECAS_UTIL_TIMER_HPP_
