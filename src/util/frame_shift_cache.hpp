/*!
* \brief FrameShiftCache
*     用于处理帧长和帧移大小不一致时的数据缓存逻辑
*/

#ifndef ECAS_UTIL_FRAME_SHIFT_CACHE_HPP_
#define ECAS_UTIL_FRAME_SHIFT_CACHE_HPP_

namespace ecas {
namespace util {

class FrameShiftCache {
    public:
    FrameShiftCache(int frame_size, int frame_shift_size);
    ~FrameShiftCache();
    //在is_ready为true后，可以使用data。使用一次后需要Pop()
    inline char *data() { return data_; }
    inline bool is_ready() { return pushed_size_ >= frame_size_? true : false; }

    // push数据;如空间有余，则返回true，否则为false
    bool Push(const char *data, const int size); //在使用后需要pop数据，pop一次减少一次帧移数据
    void Pop(); 
    void Reset();

    public:
    int frame_size_;
    int frame_shift_size_;
    int capaticy_;
    char *data_;
    int pushed_size_;
};

} // util.
} // ecas.
#endif //ECAS_UTIL_FRAME_SHIFT_CACHE_HPP_
