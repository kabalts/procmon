# 编译器配置
CC = gcc
# 编译参数：开启所有警告、额外警告，-Iinclude 需确保 include 目录存在（若不存在可删除）
CFLAGS = -Wall -Wextra -Iinclude
# 源文件列表
SRC = src/main.c src/process.c src/log.c src/daemon.c src/config.c src/uninstall.c
# 目标文件（自动将 .c 替换为 .o）
OBJ = $(SRC:.c=.o)
# 最终生成的可执行文件
TARGET = procmon

# 默认目标：编译生成 procmon
all: $(TARGET)

# 链接目标文件生成可执行文件
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

# 清理编译产物（仅清理本地文件，不涉及系统目录）
clean:
	rm -f $(OBJ) $(TARGET)
	@echo "✅ 本地编译产物已清理完成"

# 安装：将程序、配置文件部署到系统目录（需 root 权限）
install: $(TARGET)
	@if [ "$$(id -u)" -ne 0 ]; then \
		echo "❌ 安装需要 root 权限，请使用 sudo make install"; \
		exit 1; \
	fi
	cp $(TARGET) /usr/local/bin/
	chmod 755 /usr/local/bin/$(TARGET)  # 确保可执行权限
	cp -n procmon.conf /etc/procmon.conf  # -n 避免覆盖已有配置
	touch /var/log/procmon.log
	chmod 666 /var/log/procmon.log
	@echo "✅ procmon 安装完成："
	@echo "  - 程序路径：/usr/local/bin/$(TARGET)"
	@echo "  - 配置文件：/etc/procmon.conf"
	@echo "  - 日志文件：/var/log/procmon.log"

# 卸载：清理系统目录中的程序、配置、日志（需 root 权限）
uninstall:
	@if [ "$$(id -u)" -ne 0 ]; then \
		echo "❌ 卸载需要 root 权限，请使用 sudo make uninstall"; \
		exit 1; \
	fi
	# 删除可执行文件
	rm -f /usr/local/bin/$(TARGET)
	# 备份并删除配置文件（避免误删用户修改的配置）
	if [ -f /etc/procmon.conf ]; then \
		cp /etc/procmon.conf /etc/procmon.conf.bak; \
		rm -f /etc/procmon.conf; \
		echo "⚠️  配置文件已备份至 /etc/procmon.conf.bak 并删除原文件"; \
	fi
	# 清理日志文件（可选：若需保留日志可注释此行）
	rm -f /var/log/procmon.log
	# 清理本地编译产物
	$(MAKE) clean
	@echo "✅ procmon 卸载完成"

# 伪目标：避免与同名文件冲突
.PHONY: all clean install uninstall