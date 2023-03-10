#include "log.hpp"

// 主函数（测试）
int main() {

	init_log("testname.txt", LOG_LEVEL_ERROR);

	LOG_DEBUG("This is a debug message");
	LOG_INFO("This is an info message");
	LOG_WARN("This is a warning message");
	LOG_ERROR("This is an error message");
	LOG_CRITICAL("This is a critical message");

    // 销毁单例对象
    // Log::destroy();
	
	system("pause");
	return 0;
}