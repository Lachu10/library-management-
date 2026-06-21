CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = library_system
SRCS = main.c library.c utils.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /f /q $(TARGET).exe $(OBJS) 2>nul || rm -f $(TARGET) $(OBJS)
