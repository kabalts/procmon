#include "../include/procmon.h"

//  获取系统中所有进程的信息，存储在proc_list数组中
int list_all_process(ProcInfo *proc_list, int *count) 
{
    // 打开/proc目录
    DIR *dir = opendir("/proc");
    if (!dir) return -1;

    // 定义目录条目结构体指针和索引变量
    struct dirent *entry;
    int idx = 0;

    // 遍历/proc目录中的所有条目直到读完或达最大进程数 MAX_PROC
    while ((entry = readdir(dir)) != NULL && idx < MAX_PROC) 
    {
        //  只处理以数字命名的目录，即进程目录，并把目录名（字符串）转成整数 PID
        if (!isdigit(*entry->d_name)) continue;
        pid_t pid = atoi(entry->d_name);

        // 构造路径 /proc/PID/stat 来保存进程的状态信息，并打开该文件读取内容
        char path[128];
        snprintf(path, sizeof(path), "/proc/%d/stat", pid);
        FILE *fp = fopen(path, "r");
        if (!fp) continue;

        // 从文件中读取一行文本，包含进程的基本信息，如 PID、名称、状态和父进程 PID ，并关闭文件
        char buf[MAX_BUF];
        fgets(buf, sizeof(buf), fp);
        fclose(fp);

        //  解析读取的文本，提取出进程的 PID、名称、状态和父进程 PID，并存储在 ProcInfo 结构体中
        ProcInfo p;
        sscanf(buf, "%d %s %s %d", &p.pid, p.name, p.state, &p.ppid);

        //  去掉进程名称两端的括号
        p.name[strlen(p.name)-1] = 0;
        memmove(p.name, p.name+1, strlen(p.name));

        //  构造路径 /proc/PID/status 来保存进程的状态信息，并打开该文件读取内容
        snprintf(path, sizeof(path), "/proc/%d/status", pid);
        fp = fopen(path, "r");
        if (fp) 
        {
            //  读取文件内容，查找包含 "Uid:" 的行，并从中提取出进程的用户 UID ，存储在 ProcInfo 结构体中，并关闭文件
            while (fgets(buf, sizeof(buf), fp)) 
            {
                if (strstr(buf, "Uid:")) 
                {
                    sscanf(buf, "Uid:\t%u", &p.uid);
                    break;
                }
            }
            fclose(fp);
        }

        // 将解析得到的进程信息存储在 proc_list 数组中，并更新索引变量
        proc_list[idx++] = p;
    }

    // 将实际获取到的进程数量存储在 count 指针指向的变量中，并关闭目录
    *count = idx;
    closedir(dir);

    return 0;
}

//  杀死指定 PID 进程
int kill_process(pid_t pid) 
{
    if (kill(pid, SIGTERM) == 0)    // 发送 SIGTERM 信号请求进程优雅退出
    {
        sleep(1);
        kill(pid, SIGKILL);   //  等待一段时间后发送 SIGKILL 信号强制终止进程，以确保进程被完全杀死

        return 0;
    }

    return -1;
}

//  根据进程名称查找并杀死所有匹配的进程
int kill_process_by_name(const char *name) 
{
    pid_t pids[MAX_PROC];
    int cnt = 0;
    find_process_by_name(name, pids, &cnt);

    // 杀死所有匹配的进程，并返回被杀死的进程数量
    for (int i=0; i<cnt; i++) 
    {
        kill_process(pids[i]);
    }
    return cnt;
}

//  按名字找进程
int find_process_by_name(const char *name, pid_t *pid_list, int *count) 
{
    // 获取所有进程
    ProcInfo list[MAX_PROC];
    int total = 0;
    list_all_process(list, &total);

    // 遍历所有进程，检查进程名称是否包含指定的字符串，如果匹配则将该进程的 PID 存储在 pid_list 数组中，并更新索引变量
    int idx = 0;
    for (int i=0; i<total; i++) 
    {
        if (strstr(list[i].name, name)) 
        {
            pid_list[idx++] = list[i].pid;
        }
    }

    // 将匹配到的进程数量存储在 count 指针指向的变量中，并返回匹配到的进程数量
    *count = idx;
    return idx;
}

//  检查进程列表中有多少个僵尸进程
int check_zombie_process(ProcInfo *proc_list, int count) 
{
    int zombies = 0;

    // 遍历进程列表，找出僵尸进程，统计它们的数量，最后返回统计结果
    for (int i=0; i<count; i++) 
    {
        if (proc_list[i].state[0] == 'Z') zombies++;
    }
    return zombies;
}

//  杀死所有僵尸进程
int kill_all_zombies(void) 
{
    ProcInfo list[MAX_PROC];
    int count = 0;
    memset(list, 0, sizeof(list));

    // 获取所有进程列表
    list_all_process(list, &count);
    int cleaned = 0;

    for (int i = 0; i < count; i++) 
    {
        // 判断是否为僵尸进程
        if (list[i].state[0] == 'Z' || strcmp(list[i].state, "Zombie") == 0) 
        {
            int ppid = list[i].ppid;
            if (ppid <= 1) 
            {
                continue; // init/systemd 会自动回收，跳过
            }

            // 给父进程发送 SIGCHLD，让它主动回收僵尸
            if (kill(ppid, SIGCHLD) == 0) 
            {
                cleaned++;
            }
        }
    }

    return cleaned;
}