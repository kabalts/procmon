#include "../include/procmon.h"

int uninstall_procmon(void) {
    char ch;

    printf("\n===================================\n");
    printf("        卸载 procmon 工具\n");
    printf("===================================\n");
    printf("确定要卸载吗？[Y/n]：");

    scanf(" %c", &ch);
    if (ch != 'Y' && ch != 'y') {
        printf("卸载已取消。\n");
        return 0;
    }

    // 停止进程
    printf("正在停止 procmon 进程...\n");
    system("sudo pkill -f procmon 2>/dev/null");

    // 删除文件
    printf("正在删除程序...\n");
    system("sudo rm -f /usr/local/bin/procmon");
    system("sudo rm -f /etc/procmon.conf");
    system("sudo rm -f /var/log/procmon.log");

    printf("卸载完成！\n");
    return 0;
}