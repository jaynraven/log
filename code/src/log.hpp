#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <chrono>
#include <ctime>
#include <thread>
#include <stdarg.h>
#include <memory>
#include <corecrt_io.h>


// 定义日志级别
enum LOG_LEVEL
{
	LOG_LEVEL_DEBUG = 1,
	LOG_LEVEL_INFO,
	LOG_LEVEL_WARN,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_CRITICAL
};

enum LOG_MODE
{
    LOG_MODE_TERMINAL = 0x01,
    LOG_MODE_FILE = 0x02
};

// 定义日志接口宏
#define LOG_DEBUG(format, ...) Log::getInstance().write(LOG_LEVEL_DEBUG, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) Log::getInstance().write(LOG_LEVEL_INFO, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...) Log::getInstance().write(LOG_LEVEL_WARN, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) Log::getInstance().write(LOG_LEVEL_ERROR, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define LOG_CRITICAL(format, ...) Log::getInstance().write(LOG_LEVEL_CRITICAL, __FILE__, __LINE__, format, ##__VA_ARGS__)

std::string filename;
LOG_LEVEL log_level;
int log_mode;
int logfile_max_size = 10 * 1024* 1024; //默认最大10mb，大于10mb只保留最新的日志

void init_log(
	const std::string& filename_in,
	const int& log_mode_in,
	const LOG_LEVEL& log_level_in,
	const int& logfile_max_size_in = 10 * 1024 * 1024)
{
	filename = filename_in;
	log_mode = log_mode_in;
	log_level = log_level_in;
	logfile_max_size = logfile_max_size_in;
}

class Log {
private:
    FILE* file_;  // 文件指针
    std::mutex mtx_;  // 互斥锁
    uint64_t file_size_;

    // 禁止复制构造函数
    Log(Log const&);
    Log& operator=(Log const&) {}

    // 私有构造函数
    Log() {
        // 打开或新建日志文件
        file_ = fopen(filename.c_str(), "a");
        file_size_ = filelength(fileno(file_));
        int i = 0;
    }

    bool scroll_file() //删除当前目录下的.1文件，把当前文件重命名为.1
	{
		std::string new_filename = filename + ".1";
        if (_access(new_filename.c_str(), 0) == 0)
		{
            if (remove(new_filename.c_str()) != 0)
                return false;
		}

		fclose(file_);
        file_ = NULL;
        if (rename(filename.c_str(), new_filename.c_str()) == 0)
		{
			file_ = fopen(filename.c_str(), "a");
			return true;
        }
        else
            return false;
    }

public:

    ~Log() {
        if (file_ != NULL)
		{
			fclose(file_);
            file_ = NULL;
        }
    }

    // 获取单例对象
    static Log& getInstance() {
        static Log instance;
        return instance;
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

        if (log_mode & LOG_MODE_FILE)
		{
			if (file_size_ + logmsg.size() > logfile_max_size)
			{
				scroll_file();
				file_size_ = 0;
			}

			// 写入日志
			fwrite(logmsg.c_str(), logmsg.size(), 1, file_);
			fflush(file_);

            file_size_ += logmsg.size();
        }
        if (log_mode & LOG_MODE_TERMINAL)
        {
            // 写入终端
            printf(logmsg.c_str());
        }

        // 解锁
        mtx_.unlock();
    }
};
