/*!
* \brief FrameShiftCache.
*/

#include "frame_shift_cache.hpp"

#include <string.h>
#include "util/logger.hpp"

namespace ecas {
namespace util {

FrameShiftCache::FrameShiftCache(int frame_size, int frame_shift_size) {
    frame_size_ = frame_size;
    frame_shift_size_ = frame_shift_size;
    capaticy_ = frame_size *2;// 两倍空间

    data_ = (char *)malloc(capaticy_);
    pushed_size_ = 0;
}

FrameShiftCache::~FrameShiftCache() {
    free(data_);
}

bool FrameShiftCache::Push(const char *data, const int size) {
    if (size > capaticy_ - pushed_size_)
        return false;
    memcpy(data_ + pushed_size_, data, size);
    pushed_size_ += size;
    return true;
}

void FrameShiftCache::Pop() {
    // printf("is_ready: %d.\n", pushed_size_ >= frame_size_? true : false);
    pushed_size_ -= frame_shift_size_;
    // printf("pop: %d, %d, %d.\n", data, data  + frame_shift size , pushed size_);
    memmove(data_, data_ + frame_shift_size_, pushed_size_);
}

void FrameShiftCache::Reset() {
    memset(data_, 0, frame_size_);
    pushed_size_ = 0;
}

} // util.
} // ecas.
