CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
SRC = src/main.c src/process.c src/log.c src/daemon.c src/config.c src/uninstall.c
OBJ = $(SRC:.c=.o)
TARGET = procmon

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)

install:
	cp $(TARGET) /usr/local/bin/
	cp procmon.conf /etc/procmon.conf
	touch /var/log/procmon.log
	chmod 666 /var/log/procmon.log