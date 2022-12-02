/*!
* \brief Buffer. 
*/

#ifndef DLAS_CORE_BUFFER_HPP_
#define DLAS_CORE_BUFFER_HPP_

#include <string>
#include <vector>

namespace dlas {

class Buffer {

public:
    Buffer();
    ~Buffer();

private:
    int size_;
    int len_;
    void *data_;
};

}  // end of namespace dlas.

#endif // DLAS_CORE_BUFFER_HPP_