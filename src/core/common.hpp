/*!
* \brief common.
*/

#ifndef DLAS_CORE_COMMON_HPP_
#define DLAS_CORE_COMMON_HPP_

#include <iostream>
#include "logging.hpp"

namespace dlas {

// Get type from type flag.
#define TYPE_SWITCH(type, DType, ...)                   \
    switch (type) {                                     \
    case dlas::TypeFlag::FLOAT32:                       \
        {                                               \
            typedef float DType;                        \
            {__VA_ARGS__}                               \
        }                                               \
        break;                                          \
    case dlas::TypeFlag::INT32:                         \
        {                                               \
            typedef int32_t DType;                      \
            {__VA_ARGS__}                               \
        }                                               \
        break;                                          \
    case dlas::TypeFlag::INT8:                          \
        {                                               \
            typedef int8_t DType;                       \
            {__VA_ARGS__}                               \
        }                                               \
        break;                                          \
    default:                                            \
        LOG(ERROR) << "Unknown type enum " << type;     \
    }

// Get type flag from type.
template<typename DType>
struct DataType;
template<>
struct DataType<float> {
    static const int kFlag = dlas::TypeFlag::FLOAT32;
};
template<>
struct DataType<int32_t> {
    static const int kFlag = dlas::TypeFlag::INT32;
};
template<>
struct DataType<uint32_t> {
    static const int kFlag = dlas::TypeFlag::INT32;
};
template<>
struct DataType<int8_t> {
    static const int kFlag = dlas::TypeFlag::INT8;
};
template<>
struct DataType<uint8_t> {
    static const int kFlag = dlas::TypeFlag::INT8;
};

} // dlas.

#endif //DLAS_CORE_COMMON_HPP_
