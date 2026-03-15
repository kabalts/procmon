#include "../include/procmon.h"

// 写日志到文件
int log_message(const char *level, const char *msg) 
{

    FILE *fp = fopen(LOG_FILE, "a+");   //以追加模式打开日志文件

    if (!fp) return -1; // 打开日志文件失败

    //获取当前系统时间并格式化为标准格式
    time_t now = time(NULL);    
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));

    // 写入日志信息
    fprintf(fp, "[%s] [%s] %s\n", time_str, level, msg);
    fclose(fp);

    return 0;
}