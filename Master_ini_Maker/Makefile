# 編譯器與參數
CC = g++
CFLAGS = -std=c++17

# 檔案設定
SRCS = main.cpp 
OBJS = $(SRCS:.cpp=.o)

# 最終目標執行檔
TARGET = main

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
