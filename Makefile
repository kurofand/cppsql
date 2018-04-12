CC=g++

CFLAGS=-std=c++11 -lmysqlcppconn

all:
	$(CC) $(CFLAGS) main.cpp -o sqlTest
