#include "../include/procmon.h"

// 主界面
void show_help(void) {
    printf("=== procmon 高级进程管理工具 ===\n");
    printf("  -l, --list          显示所有进程\n");
    printf("  -l -u                仅显示用户进程\n");
    printf("  -l -s                仅显示系统进程\n");
    printf("  -k, --kill PID       杀死进程\n");
    printf("  -K, --killall 名称   批量杀死进程\n");
    printf("  -z, --zombie         查看僵尸数量\n");
    printf("  -Z, --killzombie     清理所有僵尸\n");
    printf("  -d, --daemon         后台守护\n");
    printf("  --uninstall          卸载procmon(请在root权限下运行,即sudo procmon --uninstall)\n");
    printf("  -h, --help           帮助\n");
}

// 监控进程
void monitor_process(MonitorConfig *cfg) {
    log_message("INFO", "进程监控启动");
    while (1) {
        pid_t pid_list[100];
        int cnt = 0;
        find_process_by_name(cfg->monitor_name, pid_list, &cnt);

        if (cnt == 0 && cfg->auto_restart && cfg->restart_count < cfg->max_restart) {
            char msg[128];
            snprintf(msg, sizeof(msg), "进程[%s]崩溃，自动重启", cfg->monitor_name);
            log_message("WARN", msg);
            system(cfg->start_cmd);
            cfg->restart_count++;
        }
        sleep(cfg->check_interval);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) 
    { 
        show_help(); 

        return 0; 
    }

    int show_user = 0, show_system = 0, show_zombie = 0;

    if (argc >= 3) {
        if (!strcmp(argv[1], "-l")) {
            if (!strcmp(argv[2], "-u")) show_user = 1;
            if (!strcmp(argv[2], "-s")) show_system = 1;
            if (!strcmp(argv[2], "-z")) show_zombie = 1;
        }
    }

    if (!strcmp(argv[1], "-l")) {
        ProcInfo list[MAX_PROC];
        int cnt = 0;
        list_all_process(list, &cnt);

        // 僵尸进程列表专用表头
        if (show_zombie) {
            printf("==== 僵尸进程详细列表 ====\n");
            printf("%-8s %-12s %-8s %-10s %-8s\n", "PID", "名称", "状态", "类型", "父进程PID");
            printf("-----------------------------------------------\n");
        } else {
            // 普通进程列表表头
            printf("%-8s %-12s %-8s %-10s\n", "PID", "名称", "状态", "类型");
            printf("---------------------------------------------\n");
        }

        int zombie_count = 0;
        for (int i=0; i<cnt; i++) {
            int is_sys = (list[i].uid == 0);
            
            // 过滤逻辑
            if (show_user && is_sys) continue;
            if (show_system && !is_sys) continue;
            if (show_zombie && list[i].state[0] != 'Z') continue;

            const char *type = is_sys ? "系统进程" : "用户进程";
            if (show_zombie) {
                // 僵尸进程额外显示父进程PID
                printf("%-8d %-12s %-8s %-10s %-8d\n", 
                       list[i].pid, list[i].name, list[i].state, type, list[i].ppid);
                zombie_count++;
            } else {
                printf("%-8d %-12s %-8s %-10s\n", 
                       list[i].pid, list[i].name, list[i].state, type);
            }
        }

        // 僵尸进程列表末尾显示统计
        if (show_zombie) {
            printf("-----------------------------------------------\n");
            printf("僵尸进程总数：%d\n", zombie_count);
            if (zombie_count == 0) {
                printf("提示：当前系统无僵尸进程 ✅\n");
            }
        }
        return 0;
    }

    if (!strcmp(argv[1], "--uninstall")) {
        uninstall_procmon();
        return 0;
    }

    if (!strcmp(argv[1], "-k") && argc == 3) {
        pid_t pid = atoi(argv[2]);
        kill_process(pid);
        printf("已杀死：%d\n", pid);
    }
    else if (!strcmp(argv[1], "-K") && argc == 3) {
        int n = kill_process_by_name(argv[2]);
        printf("已杀死 %d 个 %s 进程\n", n, argv[2]);
    }
    else if (!strcmp(argv[1], "-z")) {
        ProcInfo list[MAX_PROC];
        int cnt = 0;
        list_all_process(list, &cnt);
        printf("僵尸进程数量：%d\n", check_zombie_process(list, cnt));
    }
    else if (!strcmp(argv[1], "-Z")) {
        int num = kill_all_zombies();
        printf("已清理僵尸：%d\n", num);
        log_message("INFO", "一键清理僵尸");
    }
    else if (!strcmp(argv[1], "-d")) {
        MonitorConfig cfg;
        memset(&cfg, 0, sizeof(cfg));
        load_config(&cfg);
        create_daemon();
        monitor_process(&cfg);
    }
    else if (!strcmp(argv[1], "-h")) {
        show_help();
    }

    return 0;
}