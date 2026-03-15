#include "../include/procmon.h"

int load_config(MonitorConfig *cfg) {
    FILE *fp = fopen(CONFIG_FILE, "r");
    if (!fp) return -1;

    char buf[MAX_BUF];
    while (fgets(buf, sizeof(buf), fp)) {
        if (buf[0] == '#' || strlen(buf) < 3) continue;
        if (strstr(buf, "process_name")) sscanf(buf, "process_name=%s", cfg->monitor_name);
        if (strstr(buf, "auto_restart")) sscanf(buf, "auto_restart=%d", &cfg->auto_restart);
        if (strstr(buf, "interval")) sscanf(buf, "interval=%d", &cfg->check_interval);
        if (strstr(buf, "max_restart")) sscanf(buf, "max_restart=%d", &cfg->max_restart);
    }

    fclose(fp);
    return 0;
}