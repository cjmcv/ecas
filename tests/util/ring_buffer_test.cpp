/*!
* \brief . 
*/

#include "util/ring_buffer.hpp"

#include "gtest/gtest.h"

namespace {

using namespace ecas::util;

void RingBufferTest() {
    int len = 1000;
    RingBuffer *rb = new RingBuffer(len*sizeof(float));
    float *in_data = new float[len];
    for (int i=0; i<len; i++)
        in_data[i] = i;
    
    int loop_cnt = 100;
    int each_iter_size = len / loop_cnt * sizeof(float);
    for (int i=0; i<loop_cnt; i++)
        rb->Write((char *)in_data + i*each_iter_size, each_iter_size);

    float *out_data = new float[len];
    for (int i=0; i<loop_cnt; i++)
        rb->Read((char *)out_data + i*each_iter_size, each_iter_size);

    for (int i=0; i<len; i++)
        EXPECT_EQ(out_data[i], i);

    delete[] in_data;
    delete[] out_data; 
    delete rb;
}

TEST(UtilTest, RingBuffer) {
    RingBufferTest();
}

}  // end of namespace.