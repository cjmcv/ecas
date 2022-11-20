/*!
* \brief Log.
*/

#ifndef DLAS_UTIL_LOG_HPP_
#define DLAS_UTIL_LOG_HPP_

#include <iostream>
#include <sstream>

namespace dlas {
namespace util {

enum LogLevel {
    INFO_S = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3,
};

class LogMessage : public std::basic_ostringstream<char> {
public:
    LogMessage(const char* fname, int line, int severity)
        : fname_(fname), line_(line), severity_(severity) {}
    ~LogMessage() {
        if (severity_ >= min_log_level_) GenerateLogMessage();
    }

private:
    void GenerateLogMessage() {
        fprintf(stderr, "<%c>", "IIWE"[severity_]);
        if (fname_) {
            fprintf(stderr, " %s:%d] ", fname_, line_);
        }
          fprintf(stderr, "%s.", str().c_str());

        if (severity_ != INFO_S)
            fprintf(stderr, "\n");

        if (severity_ == ERROR)
            std::abort();
    }

public:
    static int min_log_level_;

private:
    const char* fname_;
    int line_;
    int severity_;
};

int LogMessage::min_log_level_ = WARNING;

#define LOG(severity) _DLAS_LOG_##severity

#define _DLAS_LOG_INFO_S    LogMessage(nullptr, 0, LogLevel::INFO_S)
#define _DLAS_LOG_INFO      LogMessage(nullptr, 0, LogLevel::INFO)
#define _DLAS_LOG_WARNING   LogMessage(__FILE__, __LINE__, LogLevel::WARNING)
#define _DLAS_LOG_ERROR     LogMessage(__FILE__, __LINE__, LogLevel::ERROR)

} // util
} // dlas

#endif // DLAS_UTIL_LOG_HPP_