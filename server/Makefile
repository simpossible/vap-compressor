# 指定编译器
CC = gcc

# 指定源文件
SRCS = $(wildcard ./src/**/*.c)

# 指定目标文件
OBJS = $(patsubst %.c,%.o,${SRCS})

# 指定链接库
LIB = libserver.a

# 指定头文件目录
INCLUDES = -Isrc -I../include/cjson -I../include/codec -I../include/cvetweb -I../include/codec

# 默认目标
all: $(LIB)

# 规则来生成.o文件
${OBJS}:%.o:%.c
	$(CC) $(INCLUDES) -c $< -o $@

# 规则来生成链接库
$(LIB): $(OBJS)
	ar rcs $@ $^

# 清理目标
clean:
	rm -f $(OBJS) $(LIB)