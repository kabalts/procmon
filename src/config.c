#include "../include/procmon.h"

// 加载配置文件
int load_config(MonitorConfig *cfg) 
{
    FILE *fp = fopen(CONFIG_FILE, "r"); // 打开配置文件
    
    if (!fp) return -1; // 打开配置文件失败

    char buf[MAX_BUF];  //定义一个缓冲区
    
    while (fgets(buf, sizeof(buf), fp)) // 逐行读取配置文件
    {
        if (buf[0] == '#' || strlen(buf) < 3) continue; // 跳过注释行和空行

        // 读取配置项并存储到cfg结构体中
        if (strstr(buf, "process_name")) sscanf(buf, "process_name=%[^\n]", cfg->monitor_name);
        if (strstr(buf, "start_command")) sscanf(buf, "start_command=%[^\n]", cfg->start_cmd);
        if (strstr(buf, "auto_restart")) sscanf(buf, "auto_restart=%d", &cfg->auto_restart);
        if (strstr(buf, "interval")) sscanf(buf, "interval=%d", &cfg->check_interval);
        if (strstr(buf, "max_restart")) sscanf(buf, "max_restart=%d", &cfg->max_restart);
    }

    // 关闭文件
    fclose(fp);

    return 0;
}