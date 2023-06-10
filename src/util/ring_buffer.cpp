/*!
* \brief RingBuffer.
*/

#include "ring_buffer.hpp"
#include "util/logger.hpp"
#include "string.h"

namespace ecas {
namespace util {

RingBuffer::RingBuffer(unsigned int size) {
    data_ =(unsigned char *)malloc(size);
    if (data_ == nullptr) {
        ECAS_LOGE("RingBuffer::RingBuffer:malloc for data fail!n");
    }

    start_ = data_;
    end_ = start_ + size;
    capacity_ = size;

    head_ = start_;
    tail_ = start_;
    payload_= 0;

    pthread_mutex_init(&mutex_, nullptr);
    pthread_cond_init(&not_full_,nullptr);
    pthread_cond_init(&not_empty_,nullptr);
}

RingBuffer::~RingBuffer() { 
    if(data_ != nullptr) {
        free(data_);
        data_ = nullptr;
    }
}

uint32_t RingBuffer::GetPayloadSize() {
    uint32_t size = 0;
    pthread_mutex_lock(&mutex_);
    size = payload_;
    pthread_mutex_unlock(&mutex_);
    return size;
}

uint32_t RingBuffer::GetFreeSize(){ 
    uint32_t size = 0;
    pthread_mutex_lock(&mutex_);
    size = capacity_ - payload_;
    pthread_mutex_unlock(&mutex_);
    return size;
}

bool RingBuffer::Write(char *data,const uint32_t size, bool is_blocking) {
    if ((nullptr == data) || (0 == size) || (size > capacity_))
        return false;

    pthread_mutex_lock(&mutex_);
    // Wait if there is not enough space left
    while (size > (capacity_ - payload_)) {
        if (is_blocking)
            pthread_cond_wait(&not_full_, &mutex_);
        else
            return false;
    }

    if(head_ >= tail_) {
        if (size <= (end_ - head_)) {
            memcpy(head_, data, size);
            head_ += size;
        }
        else {
            unsigned int tmp_size = end_ - head_;
            memcpy(head_, data, tmp_size);
            head_ = start_;
            memcpy(head_, data + tmp_size, size - tmp_size);
            head_ += size - tmp_size;
        }
    }
    else if (head_ < tail_) {
        memcpy(head_, data, size);
        head_ += size;
    }
    if(head_ >= end_) {
        head_ = start_;
    }
    payload_ += size;

    pthread_mutex_unlock(&mutex_);
    pthread_cond_signal(&not_empty_);
    return true;
}

bool RingBuffer::Read(char *data, const uint32_t size, bool is_blocking) {
    if ((0 == size) || (size > capacity_))
        return false;
        
    pthread_mutex_lock(&mutex_);
    // Wait if there is not enough available data
    while(size > payload_) {
        if (is_blocking)
            pthread_cond_wait(&not_empty_, &mutex_);
        else
            return false;
    }
    if(head_ >= tail_) {
        if (data != nullptr) memcpy(data, tail_, size);
        tail_ += size;
    }
    else if (head_ < tail_) {
        if (size <= (end_ - tail_)) {
            if (data != nullptr) memcpy(data, tail_, size);
            tail_ += size;
        }
        else {
            int tmp_size = end_ - tail_;
            if (data != nullptr) memcpy(data, tail_, tmp_size);
            tail_ = start_; 
            if (data != nullptr) memcpy(data + tmp_size, tail_, size - tmp_size);
            tail_ += size-tmp_size;
        }
    }
    if (tail_ >= end_) {
        tail_ = start_;
    }
    payload_ -= size;
    pthread_mutex_unlock(&mutex_);
    pthread_cond_signal(&not_full_);
    return true;
}

void RingBuffer::Reset() {
    pthread_mutex_lock(&mutex_);
    head_ = start_;
    tail_ = start_;
    payload_ = 0;
    pthread_mutex_unlock(&mutex_);
    pthread_cond_signal(&not_full_);
}

} // util.
} // ecas.
