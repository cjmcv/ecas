/*!
* \brief common.
*/

#ifndef ECAS_UTIL_COMMON_HPP_
#define ECAS_UTIL_COMMON_HPP_

namespace ecas {
namespace util {

////////////////
// Class.
////////////////
class StrProcessor {
public:
    static std::string FetchSubStr(std::string &src_str, std::string start_str, std::string end_str) {
        int start_idx = src_str.find(start_str) + start_str.length();
        int end_idx = src_str.find(end_str, start_idx);
        return src_str.substr(start_idx, end_idx - start_idx);
    }
};

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
