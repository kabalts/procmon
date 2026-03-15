#include "../include/procmon.h"

void show_help() {
    printf("=== procmon 高级进程管理工具 ===\n");
    printf("  -l, --list        列出所有进程\n");
    printf("  -k, --kill PID    杀死指定进程\n");
    printf("  -K, --killall 名称 批量杀死进程\n");
    printf("  -z, --zombie      检测僵尸进程\n");
    printf("  -d, --daemon      后台守护监控\n");
    printf("  -h, --help        帮助信息\n");
}

void monitor_process(MonitorConfig *cfg) {
    log_message("INFO", "进程监控启动");
    while (1) {
        pid_t pid_list[100];
        int cnt = 0;
        find_process_by_name(cfg->monitor_name, pid_list, &cnt);

        if (cnt == 0 && cfg->auto_restart && cfg->restart_count < cfg->max_restart) {
            char msg[128];
            snprintf(msg, sizeof(msg), "进程[%s]崩溃，尝试重启", cfg->monitor_name);
            log_message("WARN", msg);
            system(cfg->monitor_name);
            cfg->restart_count++;
        }
        sleep(cfg->check_interval);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) { show_help(); return 0; }

    if (!strcmp(argv[1], "-l") || !strcmp(argv[1], "--list")) {
        ProcInfo list[MAX_PROC];
        int cnt = 0;
        list_all_process(list, &cnt);
        printf("PID\t名称\t状态\n");
        for (int i=0; i<cnt; i++) {
            printf("%d\t%s\t%s\n", list[i].pid, list[i].name, list[i].state);
        }
    }
    else if (!strcmp(argv[1], "-k") && argc == 3) {
        pid_t pid = atoi(argv[2]);
        kill_process(pid);
        printf("已杀死进程：%d\n", pid);
    }
    else if (!strcmp(argv[1], "-K") && argc == 3) {
        int n = kill_process_by_name(argv[2]);
        printf("已杀死 %d 个 %s 进程\n", n, argv[2]);
    }
    else if (!strcmp(argv[1], "-z") || !strcmp(argv[1], "--zombie")) {
        ProcInfo list[MAX_PROC];
        int cnt = 0;
        list_all_process(list, &cnt);
        printf("僵尸进程数量：%d\n", check_zombie_process(list, cnt));
    }
    else if (!strcmp(argv[1], "-d") || !strcmp(argv[1], "--daemon")) {
        MonitorConfig cfg;
        memset(&cfg, 0, sizeof(cfg));
        load_config(&cfg);
        create_daemon();
        monitor_process(&cfg);
    }
    else if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
        show_help();
    }
    return 0;
}