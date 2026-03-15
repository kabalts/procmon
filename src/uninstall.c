#include "../include/procmon.h"

int uninstall_procmon() {
    printf("确定要卸载 procmon 吗？此操作不可恢复！(Y/n)：");
    char c;
    scanf("%c", &c);

    if (c != 'Y' && c != 'y') {
        printf("卸载已取消。\n");
        return 0;
    }

    printf("正在停止 procmon 守护进程...\n");
    system("sudo pkill -f procmon");

    printf("正在删除程序...\n");
    unlink("/usr/local/bin/procmon");
    unlink("/etc/procmon.conf");
    unlink("/var/log/procmon.log");

    printf("✅ 卸载成功！\n");
    return 0;
}