CC = g++
CFLAGS = -Wall -Wextra -std=c++17
LDFLAGS = -lssh

SRCS = main.cpp ssh_login.cpp
OBJS = $(SRCS:.cpp=.o)
HEADERS = ssh_login.h archangel.h
TARGET = archangel

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

main.o: main.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c main.cpp

ssh_login.o: ssh_login.cpp ssh_login.h
	$(CC) $(CFLAGS) -c ssh_login.cpp

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS)
