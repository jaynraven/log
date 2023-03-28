#include "log.hpp"

// 主函数（测试）
int main() {

	init_log("testname.txt", LOG_MODE_TERMINAL | LOG_MODE_FILE, LOG_LEVEL_ERROR);

	// while (true)
	// {
		LOG_DEBUG("This is a debug message");
		LOG_INFO("This is an info message");
		LOG_WARN("This is a warning message");
		LOG_ERROR("This is an error message");
		LOG_CRITICAL("This is a critical message");
	// }
	
	system("pause");
	return 0;
}