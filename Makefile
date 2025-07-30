CC = g++
CFLAGS = -std=c++17 -pthread -I./include -I./include/iniReader -I./include/NiDAQmx/include
LDFLAGS = -L./include/NiDAQmx/lib64/msvc -lNIDAQmx -static-libgcc -static-libstdc++

SRCS = main.cpp include/NiDAQ.cpp include/CSVWriter.cpp \
       include/iniReader/INIReader.cpp include/iniReader/ini.c
OBJS = $(SRCS:.cpp=.o)
OBJS := $(OBJS:.c=.o)

ifeq ($(OS),Windows_NT)
    RM = del /Q
    EXE = .exe
else
    RM = rm -f
    EXE =
endif

TARGET := main$(EXE)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS) $(TARGET)
