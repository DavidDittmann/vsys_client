//
// Created by david on 23.09.18.
//

#ifndef VSYS_SOCKETS_TCP_H
#define VSYS_SOCKETS_TCP_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#include <sstream>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <vector>

using namespace std;

class TCP_Client {
private:
    int host_port;
    string host_addr;
    int sock_fd;
    struct sockaddr_in host;
public:
    TCP_Client(int port, string addr);
    ~TCP_Client();
    //create endpoint for communication and set sock_fd to filedescriptor
    void openSocket();
    //connects to server on host_addr:host_port
    bool connectToHost();
    //send msg to server
    void sendData(string msg);
    //rcv data from server and store in buffer
    //block max 30 sec with select
    void recvData(int size, char* buffer);
};

#endif //VSYS_SOCKETS_TCP_H
