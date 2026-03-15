#include "../include/procmon.h"

void log_message(const char *level, const char *msg) {
    FILE *fp = fopen(LOG_FILE, "a+");
    if (!fp) return;

    time_t now = time(NULL);
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));
    fprintf(fp, "[%s] [%s] %s\n", time_str, level, msg);
    fclose(fp);
}