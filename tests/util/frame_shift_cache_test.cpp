/*!
* \brief . 
*/

#include "util/frame_shift_cache.hpp"

#include "gtest/gtest.h"

namespace {

using namespace ecas::util;

void FrameShiftCacheTest() {
    int frame_num = 25; //帧长25
    int frame_shift_num = 10; // 帧移10
    int block_num = 15; // 每帧输入15

    int len = 150;
    FrameShiftCache *fs = new FrameShiftCache(frame_num*sizeof(int), frame_shift_num*sizeof(int));
    int *in_data = new int[len];
    for (int i=0; i<len; i++)
        in_data[i] = i;
    
    int expected_out = 0;
    
    for (int i=0; i<len / block_num; i++) {
        fs->Push((char *)in_data+i*block_num*sizeof(int), block_num * sizeof(int));
        while (fs->is_ready()) {
            for (int j=0; j<frame_num; j++) {
                EXPECT_EQ(((int*)fs->data())[j], expected_out + j);
                // printf("%d, ", ((int*)fs->data())[j]);           
            }
            // printf("\n");
            expected_out += frame_shift_num;
            fs->Pop();
        }
    }
    delete[] in_data;
    delete fs;
}

TEST(UtilTest, FrameShiftCache) {
    FrameShiftCacheTest();
}

}  // end of namespace.