/*!
* \brief Logger.
*/

#ifndef DLAS_UTIL_LOGGER_HPP_
#define DLAS_UTIL_LOGGER_HPP_

#include <iostream>
#include <sstream>

namespace dlas {
namespace util {

enum LogLevel {
    INFO = 0,
    WARNING = 1,
    ERROR = 2
};

class Logger {
public:
    static inline Logger *GetInstance() {
        static Logger instance;
        return &instance;
    }

    inline char *buffer() { return buffer_; }
    void GenerateLogMessage(const char *fname, int line, int severity);

public:
    static int min_log_level_;

private:
    char buffer_[1024];
};

// #define LOG(severity) _DLAS_LOG_##severity

#define DLAS_LOG_BODY(level, format, ...)                                     \
    do {                                                                      \
        util::Logger *log = util::Logger::GetInstance();                      \
        if (level < util::Logger::min_log_level_)                             \
            break;                                                            \
        sprintf(log->buffer(), format, ##__VA_ARGS__);                        \
        log->GenerateLogMessage(__FILE__, __LINE__, level);                   \
    } while (false)

#define DLAS_LOGI(format, ...)       DLAS_LOG_BODY(util::LogLevel::INFO, format, ##__VA_ARGS__);
#define DLAS_LOGW(format, ...)    DLAS_LOG_BODY(util::LogLevel::WARNING, format, ##__VA_ARGS__);
#define DLAS_LOGE(format, ...)      DLAS_LOG_BODY(util::LogLevel::ERROR, format, ##__VA_ARGS__);

} // util
} // dlas

#endif // DLAS_UTIL_LOGGER_HPP_