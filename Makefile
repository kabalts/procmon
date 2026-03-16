# 编译器配置
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
SRC = src/main.c src/process.c src/log.c src/daemon.c src/config.c
OBJ = $(SRC:.c=.o)
TARGET = procmon

# 默认目标：编译生成可执行文件 + 自动清理.o中间产物（全程静默，仅显示提示）
all: $(TARGET)
	@rm -f $(OBJ)  # 静默删除.o文件，不打印rm命令
	@echo "✅ 编译完成！仅保留可执行文件：./$(TARGET)"

# 链接生成可执行文件（静默执行gcc，不打印编译命令）
$(TARGET): $(OBJ)
	@$(CC) $(OBJ) -o $(TARGET)

# 精简版clean：仅清理最终可执行文件（静默执行）
clean:
	@rm -f $(TARGET)  # 静默删除可执行文件
	@echo "✅ 已清理本地可执行文件：./$(TARGET)"

# 安装（系统级部署，全程静默，仅显示提示/错误）
install:
	@# 1. 先检查procmon是否存在，不存在则编译（编译后自动清.o）
	@if [ ! -f $(TARGET) ]; then \
		$(MAKE) all; \
	fi
	@# 2. 权限校验（静默执行，仅输出错误提示）
	@if [ "$$(id -u)" -ne 0 ]; then \
		echo "❌ 安装失败：需要root权限，请执行 sudo make install"; \
		exit 1; \
	fi
	@# 3. 部署可执行文件到系统目录（静默执行）
	@cp -f $(TARGET) /usr/local/bin/
	@chmod 755 /usr/local/bin/$(TARGET)
	@# 4. 部署配置文件（替换-n为--update=none消除警告，静默执行）
	@cp --update=none procmon.conf /etc/procmon.conf
	@# 5. 创建日志文件并设置权限（静默执行）
	@touch /var/log/procmon.log
	@chmod 666 /var/log/procmon.log
	@# 安装完成提示
	@echo "✅ 安装完成！"
	@echo "  - 程序路径：/usr/local/bin/$(TARGET)"
	@echo "  - 配置文件：/etc/procmon.conf（若不存在则新建）"
	@echo "  - 日志文件：/var/log/procmon.log"

# 卸载（系统级清理，全程静默，仅显示提示/错误）
uninstall:
	@# 权限校验（静默执行）
	@if [ "$$(id -u)" -ne 0 ]; then \
		echo "❌ 卸载失败：需要root权限，请执行 sudo make uninstall"; \
		exit 1; \
	fi
	@# 1. 删除系统目录的可执行文件（静默执行，仅显示结果）
	@if [ -f /usr/local/bin/$(TARGET) ]; then \
		rm -f /usr/local/bin/$(TARGET); \
		echo "✅ 删除系统可执行文件：/usr/local/bin/$(TARGET)"; \
	fi
	@# 2. 备份并删除配置文件（静默执行）
	@if [ -f /etc/procmon.conf ]; then \
		cp /etc/procmon.conf /etc/procmon.conf.bak; \
		rm -f /etc/procmon.conf; \
		echo "✅ 配置文件已备份至 /etc/procmon.conf.bak 并删除原文件"; \
	fi
	@# 3. 删除日志文件（静默执行）
	@if [ -f /var/log/procmon.log ]; then \
		rm -f /var/log/procmon.log; \
		echo "✅ 删除日志文件：/var/log/procmon.log"; \
	fi
	@# 4. 清理本地可执行文件（静默调用clean）
	@$(MAKE) clean > /dev/null 2>&1  # 屏蔽clean的输出，仅保留卸载提示
	@# 卸载完成提示
	@echo "🎉 卸载完成！所有文件已清理干净"

# 伪目标：避免与同名文件冲突
.PHONY: all clean install uninstall