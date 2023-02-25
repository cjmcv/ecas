/*!
* \brief Logger.
*/

#ifndef ECAS_UTIL_LOGGER_HPP_
#define ECAS_UTIL_LOGGER_HPP_

#include <iostream>
#include <sstream>
#include <mutex>

namespace ecas {
namespace util {

enum LogLevel {
    INFO_SIMPLE = 0,
    INFO,
    WARNING,
    ERROR
};

class Logger {
public:
    static inline Logger *GetInstance() {
        static Logger instance;
        return &instance;
    }
    static inline void SetMinLogLevel(LogLevel level) { min_log_level_ = level; }

    inline char *buffer() { return buffer_; }
    void GenerateLogMessage(const char *fname, int line, int severity);

public:
    static int min_log_level_;

private:
    Logger();
    ~Logger();
    char *buffer_;
};

#define ECAS_LOG_BODY(level, format, ...)                                     \
    do {                                                                      \
        util::Logger *log = util::Logger::GetInstance();                      \
        if (level < util::Logger::min_log_level_)                             \
            break;                                                            \
        static std::mutex m;                                                  \
        m.lock();                                                             \
        sprintf(log->buffer(), format, ##__VA_ARGS__);                        \
        log->GenerateLogMessage(__FILE__, __LINE__, level);                   \
        m.unlock();                                                           \
    } while (0)

#define ECAS_LOGS(format, ...)      ECAS_LOG_BODY(util::LogLevel::INFO_SIMPLE, format, ##__VA_ARGS__);
#define ECAS_LOGI(format, ...)      ECAS_LOG_BODY(util::LogLevel::INFO, format, ##__VA_ARGS__);
#define ECAS_LOGW(format, ...)      ECAS_LOG_BODY(util::LogLevel::WARNING, format, ##__VA_ARGS__);
#define ECAS_LOGE(format, ...)      ECAS_LOG_BODY(util::LogLevel::ERROR, format, ##__VA_ARGS__);

#define ECAS_CNT_LOGI(cnt, format, ...)           \
    do {                                          \
        static int count = 0;                     \
        count++;                                  \
        if (count >= cnt) {                       \
            ECAS_LOGI(format, ##__VA_ARGS__);     \
            count = 0;                            \
        } while (0)                               \
    }

} // util
} // ecas

#endif // ECAS_UTIL_LOGGER_HPP_