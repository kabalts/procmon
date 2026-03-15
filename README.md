# procmon - Linux 高级进程管理工具
轻量、无依赖、纯 C 语言编写的进程监控 + 管理 + 守护工具

## 功能
- 列出所有进程
- 按 PID / 名称杀死进程
- 僵尸进程检测
- 进程崩溃自动重启
- 后台守护模式
- 配置文件支持

## 编译安装
```bash
make
sudo make install
```

## 一键卸载
```bash
sudo pkill -f procmon
sudo rm -f /usr/local/bin/procmon
sudo rm -f /etc/procmon.conf
sudo rm -f /var/log/procmon.log
echo "procmon 已完全卸载"
```