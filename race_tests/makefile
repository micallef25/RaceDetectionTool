# the compiler: gcc for C program, define as g++ for C++
CC = gcc
# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
#  -v Verbose
CFLAGS  = -g -Wall
# -pthread support for threads
LDFLAGS = -lpthread -Wl,-Map=$@.map
# the build target executable:
TARGET = unsaferace

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c $(LDFLAGS)

clean:
	$(RM) $(TARGET) $(TARGET).map 
