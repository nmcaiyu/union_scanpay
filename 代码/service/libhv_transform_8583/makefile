# 编译器
CC = gcc
CPP = g++

# 公共编译选项
FLAG = -ggdb3 -Wall -Wextra -I/usr/local/include/hv -I/usr/include/zdb -I/usr/include/openssl -fpermissive 

# 目标文件
TARGET = slog.o action.o config.o main.o mysql.o iso8583engine.o cSockets.o

# 生成共享库
business.so: $(TARGET)
	$(CPP) $(FLAG) -pthread -o libhv_8583 -fvisibility=hidden $(TARGET) \
	-lhv -llog4cplus -lcurl -lzdb -lssl -lcrypto

# 生成目标文件
main.o: main.cpp
	$(CPP) $(FLAG) -c $< -o $@ -std=c++11

slog.o: slog.cpp
	$(CPP) $(FLAG) -c $< -o $@ -std=c++11

config.o: config.cpp
	$(CPP) $(FLAG) -c $< -o $@ -std=c++11
	
action.o: action.cpp
	$(CPP) $(FLAG) -c $< -o $@ -std=c++11

mysql.o: mysql.cpp
	$(CPP) $(FLAG) -c $< -o $@ -std=c++11

cSockets.o: cSockets.c
	$(CPP) $(FLAG) -c $< -o $@ -std=c++11
	
iso8583engine.o: ISO8583Engine/iso8583engine.c
	$(CPP) $(FLAG) -c $< -o $@

# 清理目标文件
.PHONY: clean
clean:
	rm -rf *.o demo
