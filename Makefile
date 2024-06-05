CC=g++

CFLAGS=-std=c++20 -lmysqlcppconn

all:
	$(CC) $(CFLAGS) main.cpp -o sqlTest
