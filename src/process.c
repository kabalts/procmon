#include "../include/procmon.h"

int list_all_process(ProcInfo *proc_list, int *count) {
    DIR *dir = opendir("/proc");
    if (!dir) return -1;

    struct dirent *entry;
    int idx = 0;

    while ((entry = readdir(dir)) != NULL && idx < MAX_PROC) {
        if (!isdigit(*entry->d_name)) continue;
        pid_t pid = atoi(entry->d_name);

        char path[128];
        snprintf(path, sizeof(path), "/proc/%d/stat", pid);
        FILE *fp = fopen(path, "r");
        if (!fp) continue;

        char buf[MAX_BUF];
        fgets(buf, sizeof(buf), fp);
        fclose(fp);

        ProcInfo p;
        sscanf(buf, "%d %s %s %d", &p.pid, p.name, p.state, &p.ppid);
        p.name[strlen(p.name)-1] = 0;
        memmove(p.name, p.name+1, strlen(p.name));
        proc_list[idx++] = p;
    }

    *count = idx;
    closedir(dir);
    return 0;
}

int kill_process(pid_t pid) {
    if (kill(pid, SIGTERM) == 0) {
        sleep(1);
        kill(pid, SIGKILL);
        return 0;
    }
    return -1;
}

int kill_process_by_name(const char *name) {
    pid_t pids[MAX_PROC];
    int cnt = 0;
    find_process_by_name(name, pids, &cnt);

    for (int i=0; i<cnt; i++) {
        kill_process(pids[i]);
    }
    return cnt;
}

int find_process_by_name(const char *name, pid_t *pid_list, int *count) {
    ProcInfo list[MAX_PROC];
    int total = 0;
    list_all_process(list, &total);

    int idx = 0;
    for (int i=0; i<total; i++) {
        if (strstr(list[i].name, name)) {
            pid_list[idx++] = list[i].pid;
        }
    }
    *count = idx;
    return idx;
}

int check_zombie_process(ProcInfo *proc_list, int count) {
    int zombies = 0;
    for (int i=0; i<count; i++) {
        if (proc_list[i].state[0] == 'Z') zombies++;
    }
    return zombies;
}