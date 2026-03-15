#include "../include/procmon.h"

//  把当前程序变成后台守护进程
int create_daemon() 
{
    pid_t pid;

    // 第一次fork
    pid = fork();
    if (pid < 0) 
    {
        // fork失败
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) 
    {
        // 父进程退出
        exit(EXIT_SUCCESS);
    }

    // 子进程成为会话组长
    if (setsid() < 0) 
    {
        perror("setsid failed");
        exit(EXIT_FAILURE);
    }

    // 第二次fork，防止重新获得终端
    pid = fork();
    if (pid < 0) 
    {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) 
    {
        exit(EXIT_SUCCESS);
    }

    // 设置文件权限掩码
    umask(0);

    // 改变工作目录到根目录
    if (chdir("/") < 0) 
    {
        perror("chdir failed");
        exit(EXIT_FAILURE);
    }

    // 关闭标准输入、输出和错误文件描述符
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // 重定向标准输入、输出和错误到/dev/null
    open("/dev/null", O_RDONLY); // 标准输入
    open("/dev/null", O_WRONLY); // 标准输出
    open("/dev/null", O_WRONLY); // 标准错误

    return 0;
}