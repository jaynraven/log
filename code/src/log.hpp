#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <chrono>
#include <ctime>
#include <thread>
#include <stdarg.h>
#include <memory>


// 定义日志级别
enum LOG_LEVEL
{
	LOG_LEVEL_DEBUG = 1,
	LOG_LEVEL_INFO,
	LOG_LEVEL_WARN,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_CRITICAL
};

// 定义日志接口宏
#define LOG_DEBUG(format, ...) Log::getInstance()->write(LOG_LEVEL_DEBUG, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) Log::getInstance()->write(LOG_LEVEL_INFO, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...) Log::getInstance()->write(LOG_LEVEL_WARN, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) Log::getInstance()->write(LOG_LEVEL_ERROR, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define LOG_CRITICAL(format, ...) Log::getInstance()->write(LOG_LEVEL_CRITICAL, __FILE__, __LINE__, format, ##__VA_ARGS__)

std::string filename;
LOG_LEVEL log_level;

void init_log(const std::string& filename_in, const LOG_LEVEL& log_level_in)
{
    filename = filename_in;
    log_level = log_level_in;
}

class Log {
private:
    FILE* file_;  // 文件指针
    static Log* instance_;  // 单例指针
    static std::mutex mtx_;  // 互斥锁

    // 禁止复制构造函数
    Log(Log const&);
    Log& operator=(Log const&) {}

    // 私有构造函数
    Log() {
        // 打开或新建日志文件
        file_ = fopen(filename.c_str(), "a");
    }

public:

    // 获取单例对象
    static Log* getInstance() {
        if (instance_ == nullptr) {
            std::lock_guard<std::mutex> lock(mtx_);
            if (instance_ == nullptr) {
                instance_ = new Log();
            }
        }
        return instance_;
    }

    static void destroy() {
		if (instance_ != nullptr) {
			delete instance_;
            instance_ = nullptr;
        }
    }

    // 写入日志
    void write(int level, const char* file, int line, const char* format, ...) {
        if (level < log_level)
            return;

        if (file_ == nullptr)
            return;

        // 格式化日志信息
        char buffer[1024];
        va_list args;
        va_start(args, format);
        vsprintf(buffer, format, args);
        va_end(args);

        // 获取当前时间
        auto current_time = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(current_time);
        tm local_time;
        localtime_s(&local_time, &time);

        // 构造时间戳
        char timestamp[128];
        strftime(timestamp, 128, "%Y-%m-%d %H:%M:%S", &local_time);

        // 构造日志级别描述
        std::string loglevel;
        switch (level) {
        case LOG_LEVEL_DEBUG:
            loglevel = "DEBUG";
            break;
        case LOG_LEVEL_INFO:
            loglevel = "INFO";
            break;
        case LOG_LEVEL_WARN:
            loglevel = "WARN";
            break;
        case LOG_LEVEL_ERROR:
            loglevel = "ERROR";
            break;
        case LOG_LEVEL_CRITICAL:
            loglevel = "CRIT";
            break;
        default:
            loglevel = "UNKWN";
            break;
        }

        // 构造日志信息
        std::string logmsg = std::string(timestamp) + " [" + loglevel + "] [" + 
            file + ":" + std::to_string(line) + "] " + buffer + "\n";

        // 加锁
        mtx_.lock();

        // 写入日志
        fwrite(logmsg.c_str(), logmsg.size(), 1, file_);
        fflush(file_);

        // 解锁
        mtx_.unlock();
    }

    // 私有析构函数
    ~Log() {
        if (file_ != nullptr)
            fclose(file_);
    }
};

// 静态初始化单例对象
std::mutex Log::mtx_;
Log* Log::instance_ = nullptr;