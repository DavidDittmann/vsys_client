#----------------------------------------------------------------------------------------------
#  VSYS BIF WS2018 Dittmann, Kreidl
#  date: 23.09.2018
#  makefile for client and server
#----------------------------------------------------------------------------------------------

CC = g++
#CFLAGS = -std=c++11 -g -D_BSD_SOURCE -D_XOPEN_SOURCE -Wall -o
CFLAGS = -std=c++11 -Wall -o

all: Client

Client: main.cpp
	echo "compiling Client: main.cpp..."
	${CC} ${CFLAGS} makeBin/Client main.cpp TCP.cpp TCP.h


clean:
	echo "cleaning binaries..."
	#rm -rf bin
	rm -f makeBin/Client

