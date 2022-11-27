/*!
* \brief Logger.
*/

#include "logger.hpp"

#ifdef DLAS_PLATFORM_ANDRIOD
#include <android/log.h>
#endif // DLAS_PLATFORM_ANDRIOD

namespace dlas {
namespace util {

void Logger::GenerateLogMessage(const char *fname, int line, int severity) {
#ifdef DLAS_PLATFORM_ANDRIOD
    if (severity == LogLevel::ERROR)
        __android_log_print(ANDROID_LOG_ERROR, "com.dlas", "[%s:%d] %s", fname, line, buffer_);
    else if (severity == LogLevel::WARNING)
        __android_log_print(ANDROID_LOG_INFO, "com.dlas", "[%s:%d] %s", fname, line, buffer_);
    else
        __android_log_print(ANDROID_LOG_INFO, "com.dlas", "[%s:%d] %s", fname, line, buffer_);
#else
    fprintf(stderr, "<%c>", "IIWE"[severity]);
    fprintf(stderr, " %s:%d] ", fname, line);
    fprintf(stderr, "%s", buffer_);
#endif // DLAS_PLATFORM_ANDRIOD

    if (severity == LogLevel::ERROR)
        std::abort();
}

int Logger::min_log_level_ = LogLevel::INFO;

} // util
} // dlas