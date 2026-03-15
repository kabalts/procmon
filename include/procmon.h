#ifndef PROCMON_H
#define PROCMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>

#define MAX_PATH 256
#define MAX_BUF 1024
#define MAX_PROC 1024
#define LOG_FILE "/var/log/procmon.log"
#define CONFIG_FILE "/etc/procmon.conf"

typedef struct {
    pid_t pid;
    pid_t ppid;
    char name[64];
    char state[16];
} ProcInfo;

typedef struct {
    char monitor_name[64];
    int auto_restart;
    int check_interval;
    int max_restart;
    int restart_count;
} MonitorConfig;

void log_message(const char *level, const char *msg);
int list_all_process(ProcInfo *proc_list, int *count);
int kill_process(pid_t pid);
int kill_process_by_name(const char *name);
int find_process_by_name(const char *name, pid_t *pid_list, int *count);
int check_zombie_process(ProcInfo *proc_list, int count);
void create_daemon();
int load_config(MonitorConfig *cfg);
void monitor_process(MonitorConfig *cfg);

#endif