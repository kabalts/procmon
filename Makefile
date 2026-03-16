# 编译器配置
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
SRC = src/main.c src/process.c src/log.c src/daemon.c src/config.c src/uninstall.c
OBJ = $(SRC:.c=.o)
TARGET = procmon

# 默认目标：编译 + 自动清理 .o 中间产物
all: $(TARGET)
	rm -f $(OBJ)
	@echo "✅ 编译完成！仅保留可执行文件：$(TARGET)"

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

# 精简版 clean：仅清理最终可执行文件（.o 已自动删）
clean:
	rm -f $(TARGET)
	@echo "✅ 已清理可执行文件：$(TARGET)"

# 安装（需 root 权限）
install: $(TARGET)
	@if [ "$$(id -u)" -ne 0 ]; then \
		echo "❌ 安装需要 root 权限，请使用 sudo make install"; \
		exit 1; \
	fi
	cp $(TARGET) /usr/local/bin/
	chmod 755 /usr/local/bin/$(TARGET)
	cp -n procmon.conf /etc/procmon.conf
	touch /var/log/procmon.log
	chmod 666 /var/log/procmon.log
	@echo "✅ procmon 安装完成"

# 卸载（需 root 权限，仍依赖 clean）
uninstall:
	@if [ "$$(id -u)" -ne 0 ]; then \
		echo "❌ 卸载需要 root 权限，请使用 sudo make uninstall"; \
		exit 1; \
	fi
	rm -f /usr/local/bin/$(TARGET)
	if [ -f /etc/procmon.conf ]; then \
		cp /etc/procmon.conf /etc/procmon.conf.bak; \
		rm -f /etc/procmon.conf; \
		echo "⚠️  配置文件已备份至 /etc/procmon.conf.bak"; \
	fi
	rm -f /var/log/procmon.log
	$(MAKE) clean
	@echo "✅ procmon 卸载完成"

.PHONY: all clean install uninstall