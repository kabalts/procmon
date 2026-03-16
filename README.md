# procmon - Linux 进程管理工具
轻量、纯 C 语言编写的进程监控 + 管理 + 守护工具

## 功能
- 列出所有进程
- 按 PID / 名称杀死进程
- 僵尸进程检测
- 进程崩溃自动重启
- 后台守护模式
- 配置文件支持

## 已知支持的系统
- Ubuntu 24.04

## 软件依赖
- gcc   13.3.0
- make  4.3
- libc6-dev 2.39

## 使用方式
```bash
make                #   编译
sudo make install   #   安装
sudo make uninstall #   卸载
```