/*!
* \brief Buffer. 
*/

#ifndef DLAS_CORE_BUFFER_HPP_
#define DLAS_CORE_BUFFER_HPP_

#include <string>
#include <vector>

namespace dlas {

// 内存管理，只管内存不管怎么用
class Buffer {

public:
    Buffer(unsigned int size);
    ~Buffer();
    inline void *data() { return data_; }

private:
    int size_;
    int len_;
    void *data_;
};

}  // end of namespace dlas.

#endif // DLAS_CORE_BUFFER_HPP_