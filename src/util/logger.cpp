/*!
* \brief Logger.
*/

#include "logger.hpp"

#ifdef ECAS_PLATFORM_ANDRIOD
#include <android/log.h>
#endif // ECAS_PLATFORM_ANDRIOD

namespace ecas {
namespace util {

Logger::Logger() {
    buffer_ = new char[1024];
}

Logger::~Logger() {
    delete[] buffer_;
}

void Logger::GenerateLogMessage(const char *fname, int line, int severity) {
#ifdef ECAS_PLATFORM_ANDRIOD
    if (severity == LogLevel::ERROR)
        __android_log_print(ANDROID_LOG_ERROR, "com.ecas", "[%s:%d] %s", fname, line, buffer_);
    else if (severity == LogLevel::WARNING)
        __android_log_print(ANDROID_LOG_INFO, "com.ecas", "[%s:%d] %s", fname, line, buffer_);
    else
        __android_log_print(ANDROID_LOG_INFO, "com.ecas", "[%s:%d] %s", fname, line, buffer_);
#else
    if (severity != LogLevel::INFO_SIMPLE) {
        fprintf(stderr, "<%c>", "IIWE"[severity]);
        fprintf(stderr, " %s:%d] ", fname, line);        
    }
    fprintf(stderr, "%s", buffer_);
#endif // ECAS_PLATFORM_ANDRIOD

    if (severity == LogLevel::ERROR)
        std::abort();
}

int Logger::min_log_level_ = LogLevel::INFO_SIMPLE;

} // util
} // ecas