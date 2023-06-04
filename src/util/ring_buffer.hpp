/*!
* \brief RingBuffer with cond.
*/

#ifndef ECAS_UTIL_RINGBUFFER_HPP_
#define ECAS_UTIL_RINGBUFFER_HPP_

#include <iostream>
#include <pthread.h>
#include <chrono>

namespace ecas {
namespace util {

class RingBuffer{
public:
    RingBuffer(uint32_t size);
    ~RingBuffer();
    bool Write(char *data, const uint32_t size, bool is_blocking = true);
    bool Read(char *data, const uint32_t size, bool is_blocking = true);
    uint32_t GetPayloadSize();
    uint32_t GetFreeSize();
    void Reset();

private:
    unsigned char *data_;// 内存起始点
    // 内存起始点
    unsigned char *start_;
    unsigned char *end_;
    // 内存结束点
    uint32_t capacity_;// 总容量
    unsigned char *head_;
    unsigned char *tail_;
    uint32_t payload_;

    pthread_cond_t not_empty_;
    pthread_cond_t not_full_;
    pthread_mutex_t mutex_;
};

} // util.
} // ecas.
#endif //ECAS_UTIL_RINGBUFFER_HPP_
