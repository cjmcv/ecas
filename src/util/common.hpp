/*!
* \brief common.
*/

#ifndef ECAS_UTIL_COMMON_HPP_
#define ECAS_UTIL_COMMON_HPP_

namespace ecas {
namespace util {

// Get type from type flag.
#define TYPE_SWITCH(type, DType, ...)               \
  switch (type) {                                   \
  case ecas::DataType::FP32:                        \
    {                                               \
      typedef float DType;                          \
      {__VA_ARGS__}                                 \
    }                                               \
    break;                                          \
  case ecas::DataType::FP16:                        \
    {                                               \
      typedef int16_t DType;                        \
      {__VA_ARGS__}                                 \
    }                                               \
    break;                                          \
  case ecas::DataType::INT32:                       \
    {                                               \
      typedef int32_t DType;                        \
      {__VA_ARGS__}                                 \
    }                                               \
    break;                                          \
  case ecas::DataType::INT16:                       \
    {                                               \
      typedef int16_t DType;                        \
      {__VA_ARGS__}                                 \
    }                                               \
    break;                                          \
  case ecas::DataType::INT8:                        \
    {                                               \
      typedef int8_t DType;                         \
      {__VA_ARGS__}                                 \
    }                                               \
    break;                                          \
  default:                                          \
    ECAS_LOGE("Unknown type enum: %d \n", type);    \
  }

} // util.
} // ecas.
#endif //ECAS_UTIL_COMMON_HPP_
