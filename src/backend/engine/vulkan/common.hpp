/*!
* \brief vulkan common. 
*/

#ifndef ECAS_BACKEND_VULKAN_COMMON_HPP_
#define ECAS_BACKEND_VULKAN_COMMON_HPP_

#include "util/logger.hpp"

// Used for validating return values of Vulkan API calls.
#define VK_CHECK(f) 	{																			\
    VkResult res = (f);																					\
    if (res != VK_SUCCESS)	{											        						\
        ECAS_LOGE("Fatal : VkResult is %d in %s at line %d\n", res,  __FILE__, __LINE__); \
    }																									\
}

namespace ecas {
namespace vulkan {

}  // end of namespace vulkan.
}  // end of namespace ecas.

#endif // ECAS_BACKEND_VULKAN_COMMON_HPP_